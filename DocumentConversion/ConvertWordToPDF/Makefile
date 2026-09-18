SAMPNAME=ConvertWordToPDF
OTHER_OBJS = $(SAMPNAME).o

include ../../dlutils/common.mak

# The Office-to-PDF SDK ships as shared libraries; this sample needs only its
# C library, so add office_to_pdf_c to the link. It loads the Modern C++
# library, the datalogics_interface interface and APDFL itself at run time.
# (Like every sample here, this one also inherits the common APDFL objects and
# link set from dlutils/common.mak; ConvertWordToPDF does not use them -- it
# drives the SDK entirely through the C ABI -- but they are left in place for
# consistency with the shared build rules.)
EXTRA_LIBS += -loffice_to_pdf_c

$(SAMPNAME).o: $(SRC)/$(SAMPNAME).cpp
