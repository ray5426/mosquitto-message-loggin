# Makefile for mosquitto_payload_logging plugin

# Edit the path to your mosquitto config.mk file
include /path/to/mosquitto/config.mk


PLUGIN_CFLAGS += -std=gnu11
PLUGIN_LDFLAGS += -lrt


.PHONY : all

PLUGIN_NAME=mosquitto_payload_logging

all : binary

binary : ${PLUGIN_NAME}.so

${PLUGIN_NAME}.so : ${PLUGIN_NAME}.c
	$(CROSS_COMPILE)$(CC) $(PLUGIN_CPPFLAGS) $(PLUGIN_CFLAGS) $(PLUGIN_LDFLAGS) -fPIC -shared $< -o $@

reallyclean : clean
clean:
	-rm -f *.o ${PLUGIN_NAME}.so

