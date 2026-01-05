SHELL := /usr/bin/bash
.PHONY: all server client run clean
MAKEFLAGS += --no-print-directory

# Absolute path of the workspace root
WORKSPACE = $(CURDIR)
BUILD_LOCATION = $(CURDIR)/build
# Export to ALL sub-makes
export WORKSPACE BUILD_LOCATION

all: Server Client

server:
	$(MAKE) -C Server

client:
	$(MAKE) -C Client

run-server:
	$(MAKE) -C Server run

run-client:
	$(MAKE) -C Client run

run: all
	@echo "Starting server..."
	@$(MAKE) -C Server run & \
	sleep 1 && \
	echo "Starting client..." && \
	$(MAKE) -C Client run

clean:
	$(MAKE) -C Server clean
	$(MAKE) -C Client clean
