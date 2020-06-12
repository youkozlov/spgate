#!/bin/sh

exit_script()
{
    echo "Операция завершилась ошибкой"
    exit 1
}

PROJNAME=spgate
CIP=192.168.1.213
Cpass=segnetics
export SSHPASS=$Cpass

cd $(dirname $(readlink -f ${0}))

if [ ! -f build/bin/$PROJNAME ];
    then
    echo "Исполняемый файл $PROJNAME не найден"
    echo "Убедитесь, что проект удачно скомпилирован."
    echo "Убедитесь, что скомпилированный исполняемый файл называется $PROJNAME"
    echo "Убедитесь, что исполняемый файл находится в папке $PWD/build/bin."
    exit $EXIT_FAILURE
fi

if [ "$CIP" = "" ];
    then
    echo "Введите IP адрес контроллера:"
    read CIP
fi


echo "Подготовка файлов для копирования"
rm -rf tmp
mkdir tmp
mkdir tmp/etc
mkdir tmp/etc/init.d
mkdir tmp/usr
mkdir tmp/usr/local
mkdir tmp/usr/local/bin
cp build/bin/spgate tmp/usr/local/bin/
if [ $? != 0 ]
    then
    exit_script
fi
cp cfg/default.ini tmp/etc/spgate.ini
if [ $? != 0 ]
    then
    exit_script
fi
cp cfg/spgate tmp/etc/init.d/
if [ $? != 0 ]
    then
    exit_script
fi


echo "Останов приложения"
sshpass -e ssh root@$CIP 'pkill -9 spgate'


echo "Копирование файлов"
cd tmp
sshpass -e scp -r . root@$CIP:/
if [ $? != 0 ]
    then
    exit_script
fi
cd ..


echo "Активация автозапуска приложения"
sshpass -e ssh root@$CIP 'update-rc.d spgate defaults'
if [ $? != 0 ]
    then
    exit_script
fi

echo "Очистка"
rm -rf tmp

echo "Файлы скопированы"
echo "Приложение будет запущено после перезапуска контроллера"
