SHELL := /usr/bin/bash
.PHONY: all server client run clean
MAKEFLAGS += --no-print-directory

# Absolute path of the workspace root
WORKSPACE = $(CURDIR)
BUILD_LOCATION = $(CURDIR)/build
# # Export to ALL sub-makes
export WORKSPACE BUILD_LOCATION

all: server client

server:
	$(MAKE) -C server

client:
	$(MAKE) -C client

run-server:
	$(MAKE) -C server run

run-client:
	$(MAKE) -C client run

run: all
	@echo "Starting server..."
	@$(MAKE) -C server run & \
	sleep 1 && \
	echo "Starting client..." && \
	$(MAKE) -C client run

clean:
	$(MAKE) -C server clean
	$(MAKE) -C client clean
