PROJECT_NAME = pawspective
NPROCS ?= $(shell nproc)
CLANG_FORMAT ?= clang-format
CPPCHECK ?= cppcheck
CLANG_TIDY ?= run-clang-tidy-18
BUILD_DIR ?= build-debug
DOCKER_IMAGE ?= ghcr.io/userver-framework/ubuntu-24.04-userver:latest
CMAKE_OPTS ?=
LINT_STEPS ?= format-check cppcheck tidy
TIDY_DB_DIR ?= build-debug
SECDIST_FILE = secdist.json
SECDIST_EXAMPLE_FILE = secdist.example.json
INCLUDE_DIRS := $(shell find src -type d -name "include" | sed 's/^/--extra-arg=-I/')
# If we're under TTY, pass "-it" to "docker run"
DOCKER_ARGS = $(shell /bin/test -t 0 && /bin/echo -it || echo)
PRESETS ?= debug release debug-custom release-custom

# Space-separated list of changed C++ files to lint.
# When set by CI (PR mode), only these files are checked; empty means no C++ changes.
# When unset entirely (push/local), all src/**/*.{cpp,hpp} are checked.
# Do NOT assign a default here — we use $(origin) to distinguish "unset" from "set to empty".

ifeq ($(origin CHANGED_FILES),undefined)
  LINT_CPP_FILES  := $(shell find src -name '*.[ch]pp' -type f)
  CPPCHECK_FILTER := --file-filter=*/src/*
else ifneq ($(strip $(CHANGED_FILES)),)
  LINT_CPP_FILES  := $(CHANGED_FILES)
  CPPCHECK_FILTER := $(foreach f,$(LINT_CPP_FILES),--file-filter=$(abspath $(f)))
else
  LINT_CPP_FILES  :=
  CPPCHECK_FILTER :=
endif

-include Makefile.local

.PHONY: all
all: test-debug test-release
$(SECDIST_FILE):
	@if [ ! -f $(SECDIST_FILE) ]; then \
		echo "Creating $(SECDIST_FILE) from example..."; \
		cp $(SECDIST_EXAMPLE_FILE) $(SECDIST_FILE); \
	fi


# Run cmake
.PHONY: $(addprefix cmake-, $(PRESETS))
$(addprefix cmake-, $(PRESETS)): cmake-%: $(SECDIST_FILE)
	cmake --preset $* $(CMAKE_OPTS)

$(addsuffix /CMakeCache.txt, $(addprefix build-, $(PRESETS))): build-%/CMakeCache.txt:
	$(MAKE) cmake-$*

# Build using cmake
.PHONY: $(addprefix build-, $(PRESETS))
$(addprefix build-, $(PRESETS)): build-%: build-%/CMakeCache.txt
	cmake --build build-$* -j $(NPROCS) --target $(PROJECT_NAME)

# Test
.PHONY: $(addprefix test-, $(PRESETS))
$(addprefix test-, $(PRESETS)): test-%: build-%/CMakeCache.txt
	cmake --build build-$* -j $(NPROCS)
	cd build-$* && ((test -t 1 && GTEST_COLOR=1 PYTEST_ADDOPTS="--color=yes" ctest -V) || ctest -V)

# Start the service (via testsuite service runner)
.PHONY: $(addprefix start-, $(PRESETS))
$(addprefix start-, $(PRESETS)): start-%:
	cmake --build build-$* -v --target start-$(PROJECT_NAME)

# Cleanup data
.PHONY: $(addprefix clean-, $(PRESETS))
$(addprefix clean-, $(PRESETS)): clean-%:
	cmake --build build-$* --target clean

.PHONY: dist-clean
dist-clean:
	rm -rf build*
	rm -rf tests/__pycache__/
	rm -rf tests/.pytest_cache/
	rm -rf .ccache
	rm -rf .vscode/.cache
	rm -rf .vscode/compile_commands.json

# Install
.PHONY: $(addprefix install-, $(PRESETS))
$(addprefix install-, $(PRESETS)): install-%: build-%
	cmake --install build-$* -v --component $(PROJECT_NAME)

.PHONY: install
install: install-release

# Format the sources
.PHONY: format format-check
# Local usage
format:
	find src -name '*.[ch]pp' -type f | xargs $(CLANG_FORMAT) -i
	find tests -name '*.py' -type f | xargs python3 -m autopep8 -i

# for CI
format-check:
ifdef LINT_CPP_FILES
	$(CLANG_FORMAT) --dry-run --Werror $(LINT_CPP_FILES)
else ifeq ($(origin CHANGED_FILES),undefined)
	$(error No C++ source files found in src/)
else
	@echo "No C++ files changed in this PR, skipping clang-format."
endif
	find tests -name '*.py' -type f | xargs python3 -m autopep8 --diff

#Static analyzers
.PHONY: tidy cppcheck lint
tidy:
	@echo "Running clang-tidy..."
ifdef LINT_CPP_FILES
	$(CLANG_TIDY) -p $(TIDY_DB_DIR) \
		-j $(NPROCS) \
		-extra-arg=-Wno-unknown-argument \
		-extra-arg=-Wno-unknown-warning-option \
		-extra-arg="-std=c++20" \
		$(abspath $(LINT_CPP_FILES))
else ifeq ($(origin CHANGED_FILES),undefined)
	$(error No C++ source files found in src/)
else
	@echo "No C++ files changed in this PR, skipping clang-tidy."
endif

cppcheck:
	@echo "Running cppcheck..."
ifdef LINT_CPP_FILES
	$(CPPCHECK) --enable=all --error-exitcode=1 \
		--project=$(BUILD_DIR)/compile_commands.json \
		--suppressions-list=.cppcheck_suppressions \
		$(CPPCHECK_FILTER)
else ifeq ($(origin CHANGED_FILES),undefined)
	$(error No C++ source files found in src/)
else
	@echo "No C++ files changed in this PR, skipping cppcheck."
endif

lint: $(LINT_STEPS)
	@echo "All lint check passed!"