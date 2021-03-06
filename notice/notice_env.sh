##export NKHOME=/home/jhdo/LAMPS/notice

##export LIBUSB_INC=/usr/include/libusb-1.0
##export LIBUSB_LIB=/usr/lib64
##export LIBUSB_LIB=/usr/lib/x86_64-linux-gnu

##ROOTHOME=/home/jhdo/root
#. $ROOTHOME/bin/thisroot.sh
######################################################3
export NKHOME=/home/n-ext/LAMPS/notice

export LIBUSB_INC=/usr/include/libusb-1.0
export LIBUSB_LIB=/usr/lib64
#export LIBUSB_LIB=/usr/lib/x86_64-linux-gnu

ROOTHOME=/home/n-ext/ROOT-v6.14.04/object
. $ROOTHOME/bin/thisroot.sh



if [ -z "${PATH}" ]; then
   PATH=$NKHOME/bin; export PATH
else
   PATH=$NKHOME/bin:$PATH; export PATH
fi

if [ -z "${LD_LIBRARY_PATH}" ]; then
   LD_LIBRARY_PATH=$NKHOME/lib:/usr/local/lib; export LD_LIBRARY_PATH
else
   LD_LIBRARY_PATH=$NKHOME/lib:/usr/local/lib:$LD_LIBRARY_PATH; export LD_LIBRARY_PATH
fi

