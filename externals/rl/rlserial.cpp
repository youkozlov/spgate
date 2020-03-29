/***************************************************************************
                          rlserial.cpp  -  description
                             -------------------
    begin                : Sat Dec 21 2002
    copyright            : (C) 2002 by Rainer Lehrig
    email                : lehrig@t-online.de

    RMOS implementation:
    Copyright            : (C) 2004 Zertrox GbR
    Written by           : Alexander Feller
    Email                : feller@zertrox.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#include "rlserial.h"

#ifdef RLUNIX
#include <stdio.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <linux/serial.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <asm/ioctls.h>
#include <errno.h>
#include <poll.h>
#include <stdarg.h>
#endif

#ifdef __VMS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <starlet.h>
#include <descrip.h>
#include <lib$routines.h>
#include <ssdef.h>
#include <iodef.h>
typedef struct
{
  short    iostat;
    unsigned short msg_len;
    int      reader_pid;
}IOSB;
#endif

#ifdef RLWIN32
#include <windows.h>
#include <stdio.h>
#endif

#ifdef RM3
#include <stdio.h>
#include <stdlib.h>
#include <rmcomp.h>
#include <rmapi.h>
#include <rio.h>
#include <drvspec.h>
#include <rm3cav.h>
//#include <clean.h>
#include <rcinc.h>
#define RTS_TIME_WAIT 0x008     /* Verzoerungszeit fuer RTS - Signal      */
#endif

#include "rlthread.h"

#define MODBUS_PAUSE	4


int debug = 1, log = 1, cnt = 0, f = 0;
#define dprintf if(debug)_printf

static char buf[65536] = { 0 };
void _printf(const char * ptr, ...)
{
    va_list va;
    va_start(va, ptr);
    if(log) {
        cnt += vsnprintf(&buf[cnt], sizeof(buf) - cnt, ptr, va);
        if(strchr(buf, '\n')) {
            syslog(LOG_INFO, buf);
            cnt = 0;
        }
    }
    else
        vprintf(ptr, va);
    va_end(va);
}


/*
static void sighandler(int sig)
{
  if(sig == SIGINT)
  {
    closeDevice();
    closeDatabase();
  }
}
*/

rlSerial::rlSerial()
{
  ttysavefd = -1;
  ttystate  = RESET;
  conn = fd = fdw  = -1;
  trace     = 0;
  pthread_mutex_init(&mutex, NULL);
//  link_stat = NULL;
  master = 0;
}

rlSerial::~rlSerial()
{
    if(conn > 0)
    close(conn);
  closeDevice();
  pthread_mutex_destroy(&mutex);
}

void rlSerial::setTrace(int on)
{
  if(on == 1) trace = 1;
  else        trace = 0;
}

int rlSerial::convertBaud(int baud)
{
    if(baud == 230400)
    return	B230400;
    else
    if(baud == 115200)
    return	B115200;
    else
    if(baud == 57600)
    return	B57600;
    else
    if(baud == 38400)
    return	B38400;
    else
    if(baud == 19200)
    return	B19200;
    else
    if(baud == 9600)
    return	B9600;
    else
    if(baud == 14400)
    return	B4000000;
    else
    if(baud == 4800)
    return	B4800;
    else
    if(baud == 2400)
    return	B2400;
    else
    if(baud == 1200)
    return	B1200;
    else
    if(baud == 600)
    return	B600;
    else
    if(baud == 300)
    return	B300;
    else
    if(baud == 150)
    return	B150;
    else
    if(baud == 75)
    return	B75;
    else
    return  B115200;
}

int rlSerial::setSettings(int _settings)
{
  struct termios buf;

  if(fd < 0) {
//    syslog(LOG_INFO, "Set settings %s", strerror(errno));
    if(reopenDevice())
    return -1;
  }
  settings = _settings;

  if(tcgetattr(fd, &save_termios) < 0) { return -1; }
  buf = save_termios;
  int oldset = buf.c_cflag;
  buf.c_cflag = settings | CLOCAL | CREAD;
  if(tcsetattr(fd, TCSAFLUSH, &buf) < 0) { return -1; }
  //if(tcsetattr(fd, TCSANOW, &buf) < 0) { return -1; }
  ttystate = RAW;
  ttysavefd = fd;
  tcflush(fd,TCIOFLUSH);
  return oldset;
}


static unsigned masks[] = { 1 << 7, 1 << 8, 1 << 9, 1 << 10 };
static const char * names[] = { "/dev/rs232", "/dev/rs485", "/dev/mrext", "/dev/mrint" };
static unsigned name_to_mask(const char * name)
{
    for(unsigned i = 0; i < sizeof(masks) / sizeof(unsigned); i++) {
    if(strcmp(name, names[i]) == 0)
        return masks[i];
    }
    return 0;
}


int rlSerial::openDevice(const char *devicename, int speed, int block, int rtscts, int bits, int stopbits, int parity)
{
#ifdef RLUNIX
  if(fd != -1) return -1;

  fd = fdw = open(devicename, O_RDWR | O_NONBLOCK);

  cfmakeraw(&buf);
  buf.c_cflag = speed | CLOCAL | CREAD;
  if(rtscts   == 1)  buf.c_cflag |= CRTSCTS;
  if(bits     == 7)  buf.c_cflag |= CS7;
  else               buf.c_cflag |= CS8;
  if(stopbits == 2)  buf.c_cflag |= CSTOPB;
  if(parity == rlSerial::ODD)  buf.c_cflag |= (PARENB | PARODD);
  if(parity == rlSerial::EVEN) buf.c_cflag |= PARENB;
  buf.c_lflag = IEXTEN; //ICANON;
  buf.c_oflag     = 0;
  buf.c_cc[VMIN]  = 1;
  buf.c_cc[VTIME] = 0;
#ifndef PVMAC
  buf.c_line      = 0;
#endif
  buf.c_iflag     = IGNBRK | IGNPAR | IXANY;
  tcflush(fd,TCIOFLUSH);
  if(tcsetattr(fd, TCSANOW, &buf) < 0) { return -1; }
  ttystate = RAW;
  ttysavefd = fd;
  if(block == 1) fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) & ~O_NONBLOCK);

  if(strstr(devicename, "usb")) {
    //close(conn);
    conn = -1;
    syslog(LOG_INFO, "USB %s %i", devicename, fd);
  }
  else {
    conn = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_CONNECTOR);
    if (conn == -1) {
        perror("MBS: Couldnt create connector");
    }
    unsigned mask = name_to_mask(devicename);
    l_local.nl_family = AF_NETLINK;
    l_local.nl_groups = mask; /* bitmask of requested groups */
    l_local.nl_pid = 0;

    if (conn > 0 && bind(conn, (struct sockaddr *)&l_local, sizeof(struct sockaddr_nl)) == -1) {
        perror("MBS: Couldnt bind to connector\n");
        close(conn);
    conn = -1;
    }
  }
#endif

  return 0;
}

int rlSerial::readChar()
{
#ifdef RLUNIX
  int  ret;
  unsigned char buf[2];

  if(fd == -1) return -1;
  ret = read(fd,buf,1);
  if(ret == 1) return buf[0];
  if(ret == 0) return -2;
  return -5;
#endif

}


int rlSerial::getByte()
{
    int  ret;
    unsigned char buf[2];

    ret = read(fd, buf, 1);
    if(ret == 1) return buf[0];
    if(ret == 0) return -2;

    return -5;
}

int rlSerial::writeChar(unsigned char uchar)
{
#ifdef RLUNIX
  int ret;
  if(fd == -1) return -1;
  if(trace == 1) printf("wc %x\n",(int)uchar);
  ret = write(fd,&uchar,1);
  if(ret < 0) return -1;
  //tcflush(fd, TCIOFLUSH);
  return ret;
#endif

}

int rlSerial::readNonblock(unsigned char * buf, int len)
{
    if(fd == -1) return -1;
    int ret, qty = 0,
            cnt = 10;                           //зададим максимальное количество циклов ожидания байт
    do {
        ret = read(fd, buf, len);               //читаем без блокировки
        if(trace) {
            for(int i = 0; i < ret; i++)
                printf("%02x ", (int)buf[i]);
            printf("\n");
            fflush(stdout);
        }
        if(ret < 0) {                           //если пришла какая-то ошибка
            //printf("\nerrno %i %s\n", errno, strerror(errno));
            //fflush(stdout);
            if(errno != EAGAIN)                 //если байтов в буфере пока нет, то придет EAGAIN
                return -5;                      //возврат если были ошибки
        }
        else {
            qty += ret;                         //в ret возвращается количество прочитанных байт
            if(ret == len)                      //если прочитали сколько хотели
                return qty;                     //выходим
            len -= ret;                         //уменьшаем ожидаемое количество байт
            buf += ret;
            cnt = 10;
        }
        struct timespec rqtp = { 0, 0 };
        rqtp.tv_nsec = 1000000 ;				//1мс на приём очередной порции данных
        clock_nanosleep(CLOCK_MONOTONIC, 0, &rqtp, NULL);
    } while(len && --cnt);                      //крутимся пока байты идут

    return qty;
}


int rlSerial::getNonBlock(unsigned char * buf, int len)
{
    if(fd == -1) return -1;
    int ret = read(fd, buf, len);		//читаем без блокировки
    if(trace) {
//	char t[256];
//	time_t now;
//	time(&now);
//	ctime_r(&now, t);
    dprintf("gn %i: ", ret);
    for(int i=0; i<len; i++) dprintf(" %02X",(int) buf[i]);
    dprintf("\n");
    }
    return ret;		//читаем без блокировки
}


int rlSerial::readBlock(unsigned char *buf, int len, int timeout)
{
#ifdef RLUNIX
  int c, retlen = 0;

    if(timeout >= 0)
    {
    if(select(timeout) == 0) return -4; // timeout
    }
  for(int i=0; i<len; i++)
  {
    if(select(mb_tout) == 0) { /*syslog(LOG_INFO, "read=%i\n", retlen);*/ return -6; }
    c = readChar();
    if(c < 0) return c;
    buf[i] = (unsigned char) c;
    retlen = i + 1;
  }
  if(retlen <= 0) return -1;
  return retlen;
#endif
}


int rlSerial::getBlock(unsigned char *buf, int len, int )
{
    int c, retlen = -6;

    for(int i = 0; i < len; i++) {
        if(select((byte_tout_ns / 1000) * 100) == 0) {
    //	    syslog(LOG_INFO, "read=%i\n", retlen); return -6;
            return retlen;
        }
        c = getByte();
        if(c < 0) return c;
        buf[i] = (unsigned char) c;
        retlen = i + 1;
    }
    if(retlen <= 0) return -1;

    return retlen;
}

int rlSerial::writeBlock(const unsigned char *buf, int len)
{
    int ret;

    if(fdw == -1) return -1;
    if(trace == 1) {
        //    char t[256];
        time_t now;
        time(&now);
        struct tm t;
        gmtime_r(&now, &t);
        dprintf("wb %i: ", len);
        //    printf("wb:");
        for(int i=0; i<len; i++) dprintf(" %02X",(int) buf[i]);
        dprintf("\n");
    }

    for(int i = 0; i < 2; i++) {
        ret = write(fdw,buf,len);
        if(ret <= 0) {
            if(errno == EAGAIN) {
                syslog(LOG_INFO, "Sleeping while %s", strerror(errno));
                sleep(1);
                continue;
            }
            ret = ret;
            syslog(LOG_INFO, "Write block %s", strerror(errno));
            reopenDevice();
        }
        else
            break;
    }
    if(ret <= 0)
        return -1;

    return ret;
}

int rlSerial::readLine(unsigned char *buf, int maxlen, int timeout)
{
  int i,c,ret;

  if(maxlen <= 1) return -1;
  ret = 0;
  buf[maxlen-1] = '\0';
  for(i=0; i<maxlen-2; i++)
  {
    ret = i;
    if(timeout > 0)
    {
      int t = select(timeout);
      if(t == 0) return -1;
    }
    c = readChar();
    if(c < 0)
    {
      buf[i] = '\0';
      ret = c;
      break;
    }
    buf[i] = (unsigned char) c;
    if(c < ' ' && c != '\t')
    {
      buf[i+1] = '\0';
      break;
    }
  }
  return ret;
}

int rlSerial::select(int timeout)
{
  if(fd < 0) {
    if(reopenDevice())
    return 0;
     settings = 0xFFFFFFFF;
  }
  struct timeval timout = {0, timeout};
  fd_set rset;
  int    ret,maxfdp1;

  /* setup sockets to read */
  maxfdp1 = fd+1;
  FD_ZERO(&rset);
  FD_SET (fd,&rset);
  if(timeout > 1000000) {
    timout.tv_sec  = timeout / 1000000;
    timout.tv_usec = timeout % 1000000;
  }

  ret = ::select(maxfdp1,&rset,NULL,NULL,timeout == -1 ? NULL : &timout);
  if(ret == 0) {
    return 0; /* timeout */
  }
  return 1;
}

int rlSerial::closeDevice()
{
#ifdef RLUNIX
  if(fd == -1) return -1;
  if(::tcsetattr(fd,TCSANOW,&save_termios) < 0) return -1;
  ::close(fd);
  ttystate = RESET;
  fd = -1;
  return 0;
#endif

}

int rlSerial::reopenDevice()
{
    //syslog(LOG_INFO, "Reopening device %i", fd);
    if(fd > 0)
        ::close(fd);
    char tmp[128] = "/dev/";
    fd = fdw = open(strcat(tmp, dev), O_RDWR | O_NONBLOCK);

    //syslog(LOG_INFO, "Opening device %s %i, errno %d", dev, fd, errno);

    if(fd < 0) {
        sleep(1);
        return -1;
    }
    if(tcsetattr(fd, TCSANOW, &buf) < 0)
        return -1;
    syslog(LOG_INFO, "Reopened device %s", dev);

    return 0;
}


void rlSerial::flush()
{
//  tcflush(fd,TCIOFLUSH);
    int cnt = 32;
    while(select(mb_tout))
    {
        if(trace) printf("f");
    if(--cnt == 0) {
        struct timespec sl = { 0, 1000000 };
        clock_nanosleep(CLOCK_REALTIME, 0, &sl, NULL);
        cnt = 32;
    }
        if(readChar() == -2) {
        syslog(LOG_INFO, "Flush %s", strerror(errno));
        if(reopenDevice() < 0)
        break;;
    }
    }
}

int rlSerial::calcTout(int _baud)
{
    if(_baud <= 0)
        _baud = 115200;
    byte_tout_ns = 1000000000 / _baud * 11; 	//13 = 1 стартовый + 8 данных + 2 стоповых + 2 запас, поскольку байты идут не друг за другом, а с паузой
    return MODBUS_PAUSE * 1000000 / _baud * 11;	//вычисляем таймаут окончания модбас посылки (~3.5 байта)
}


