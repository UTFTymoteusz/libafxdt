NAME  := libafxdt
TYPE  := lib
OPTIM := 3
FREE  := false # freestanding
NOSTD := false # nostdlib
BARE  := true # no exceptions, rtti, etc
DEBUG ?= true # debug
TRACE ?= true # allow stack tracing
LIBS  := 

CXXFLAGS := -std=c++20
INCLUDES := -I. -I./include

ifeq ($(TYPE), lib)
	COLOR := 35
else
	COLOR := 32
endif

ROOT     ?= $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
BUILD    := ./build/
BIN      := $(BUILD)$(NAME)/
DEP_DEST := $(BIN)dep/
OBJ_DEST := $(BIN)obj/

export ROOT
export DEBUG
export TRACE

INCLUDES := $(INCLUDES) $(foreach lib,$(LIBS),-I./lib/$(lib)/include )
EXCLUDES := $(foreach lib,$(LIBS),./lib/$(lib)/% )

CFILES   := $(filter-out $(EXCLUDES),$(shell find . -type f -name '*.c'))
CXXFILES := $(filter-out $(EXCLUDES),$(shell find . -type f -name '*.cpp'))

OBJS  := $(patsubst %.o, $(OBJ_DEST)%.o, $(CXXFILES:.cpp=.cpp.o))
LINKS := $(foreach lib,$(LIBS),./build/$(lib)/$(lib).a )

CXXFLAGS += -O$(OPTIM) -Wall -Wextra -pipe -lgcc $(INCLUDES)

ifeq ($(FREE), true)
	CXXFLAGS += -ffreestanding
endif

ifeq ($(NOSTD), true)
	CXXFLAGS += -nostdlib
endif

ifeq ($(DEBUG), true)
	CXXFLAGS += -g
endif

ifeq ($(BARE), true)
	CXXFLAGS += -fno-rtti
	CXXFLAGS += -fno-exceptions
	CXXFLAGS += -fno-pic
endif

ifeq ($(TRACE), true)
	CXXFLAGS += -fno-stack-protector 
	CXXFLAGS += -fno-omit-frame-pointer
endif

submake = @$(MAKE) -C $(1) $(2)

all: $(OBJS)
	$(foreach lib,$(LIBS),$(call submake, ./lib/$(lib), all))

ifeq ($(TYPE), lib)
	@$(AR) rcs -o $(BIN)$(NAME).a $(OBJS)
else
	@$(CXX) $(CXXFLAGS) $(LINKS) -o $(BIN)$(NAME) $(OBJS)
endif
	@printf '\033[0;$(COLOR)m%s\033[0m: Done building \033[0;$(COLOR)m%s\033[0m\033[0K\n' $(NAME)

include $(shell find $(DEP_DEST) -type f -name *.d)

clean:
	$(foreach lib,$(LIBS),$(call submake, ./lib/$(lib), clean))

	@rm -rf $(DEP_DEST)
	@rm -rf $(OBJ_DEST)

$(OBJ_DEST)%.cpp.o : %.cpp
	mkdir -p ${@D}
	mkdir -p $(dir $(DEP_DEST)$*)

	@printf '\033[0;$(COLOR)m%s\033[0m: Building \033[0;$(COLOR)m$(<)\033[0m\033[0K\n' $(NAME)
	@$(CXX) $(CXXFLAGS) -c $< -o $@ -MMD -MT $@ -MF $(DEP_DEST)$*.cpp.d
 