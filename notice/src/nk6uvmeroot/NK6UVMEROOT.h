/*
 *
 */
#ifndef NK6UVMEROOT_H
#define NK6UVMEROOT_H

#include "NKUSBROOT.h"

struct libusb_context;
struct libusb_device;
struct libusb_device_handle;

class NK6UVMEROOT : public NKUSBROOT {
public:

   NK6UVMEROOT(libusb_context **ctx = 0);
   virtual ~NK6UVMEROOT();

//   virtual Int_t VMEinit(libusb_context **ctx = 0);
   virtual Int_t VMEopen(Int_t sid = NK_SID_ANY, libusb_context *ctx = 0);
   virtual void VMEclose(Int_t sid = NK_SID_ANY);

   virtual Int_t VMEwrite(Int_t sid, UChar_t am, UChar_t tout, ULong_t address, ULong_t data);
   virtual ULong_t VMEread(Int_t sid, UChar_t am, UChar_t tout, ULong_t address);
   virtual Int_t VMEblockread(Int_t sid, UChar_t am, UChar_t tout,
                    ULong_t address, ULong_t nbytes, UChar_t *data);

   ClassDef(NK6UVMEROOT, 0) // nkvme wrapper class for root
};

#endif
