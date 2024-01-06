NAME := libafxdt
TYPE := lib
OPTI := 3
FREE := true # freestanding
NOST := true # nostdlib
BARE := true # no exceptions, rtti, etc
LIBS := 

GFLAGS   := -O$(OPTI) -Wall -Wextra -pipe -lgcc -g
INCLUDES := -I. -I./include

ifeq ($(TYPE), lib)
	COLOR := 35
else
	COLOR := 32
endif

BUILD    := ./build/
BIN      := $(BUILD)$(NAME)/
DEP_DEST := $(BIN)dep/
OBJ_DEST := $(BIN)obj/

MKDIR = mkdir -p
INCLUDES := $(INCLUDES) $(foreach lib,$(LIBS),-I./lib/$(lib)/include )
EXCLUDES := $(foreach lib,$(LIBS),./lib/$(lib)/% )

CFILES   := $(filter-out $(EXCLUDES),$(shell find . -type f -name '*.c'))
CXXFILES := $(filter-out $(EXCLUDES),$(shell find . -type f -name '*.cpp'))

OBJS := $(patsubst %.o, $(OBJ_DEST)%.o, $(CXXFILES:.cpp=.cpp.o))

ifeq ($(FREE), true)
	GFLAGS += -ffreestanding
endif

ifeq ($(NOST), true)
	GFLAGS += -nostdlib
endif

CXXFLAGS := $(GFLAGS) $(CXXFLAGS)   \
	-std=c++20				   		\
	-fno-rtti				   		\
	-fno-exceptions			   		\
	-fno-pic			   	   		\
	-fno-stack-protector       		\
	-fno-omit-frame-pointer    		\
	$(INCLUDES)

submake = $(MAKE) -C $(1) $(2)

all: $(OBJS)
	$(foreach lib,$(LIBS),$(call submake, ./lib/$(lib), all))

ifeq ($(TYPE), lib)
	@$(AR) rcs -o $(BIN)$(NAME).a $(OBJS)
else
	@$(CXX) $(CXXFLAGS) -o $(BIN)$(NAME) $(OBJS) $(LINKS)
endif
	@printf '\033[0;$(COLOR)m%s\033[0m: Done building \033[0;$(COLOR)m%s\033[0m\033[0K\n' $(NAME)

include $(shell find $(DEP_DEST) -type f -name *.d)

clean:
	$(foreach lib,$(LIBS),$(call submake, ./lib/$(lib), clean))

	@rm -rf $(DEP_DEST)
	@rm -rf $(OBJ_DEST)

$(OBJ_DEST)%.cpp.o : %.cpp
	@$(MKDIR) -p ${@D}
	@$(MKDIR) -p $(dir $(DEP_DEST)$*)

	@printf '\033[0;$(COLOR)m%s\033[0m: Building \033[0;$(COLOR)m$(<)\033[0m\033[0K\n' $(NAME)
	@$(CXX) $(CXXFLAGS) -c $< -o $@ -MMD -MT $@ -MF $(DEP_DEST)$*.cpp.d
 