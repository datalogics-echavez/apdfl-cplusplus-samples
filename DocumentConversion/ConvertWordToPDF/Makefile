SAMPNAME=ConvertWordToPDF
OTHER_OBJS = $(SAMPNAME).o

include ../../dlutils/common.mak

# The Office-to-PDF SDK is a static library, so this sample resolves the SDK's
# curator (Modern C++ interface), zlib and expat symbols at its own final link.
# It is compiled with O2PDF_STATIC so office_to_pdf/converter_c.h declares plain
# (non-exported) functions rather than shared-library imports.
CXXFLAGS += -DO2PDF_STATIC
EXTRA_LIBS += -loffice_to_pdf_sdk -ldatalogics_interface_api -lexpat -lz

$(SAMPNAME).o: $(SRC)/$(SAMPNAME).cpp
