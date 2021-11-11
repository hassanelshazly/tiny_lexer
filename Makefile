#------------------------------------------------------------------------------
# Makefile used to build the project
#
# Use: make [TARGET]
#
# Build Targets:
# 	   clean   :  remove all the gemerted files from
# 				  the complier
#	   All     :  The whole project
#      main.out:  The whole project
#	   %.o     :  %.cpp
#
#------------------------------------------------------------------------------
SRC_DIR = ./
OBJ_DIR = obj

$(shell [ -e obj ] ||  mkdir obj)

SOURCES  := $(wildcard ${SRC_DIR}/*.cpp)
INCLUDES :=

CXX 		 = g++
CPPFLAGS = $(INCLUDES) -MMD -MP -std=c++17

OBJS := $(SOURCES:.cpp=.o)
OBJS := $(patsubst ${SRC_DIR}/%,${OBJ_DIR}/%,$(OBJS))
	
DEPS   = $(OBJS:.o=.d)
TARGET = lexer

# Rule for genertaing .o files 
$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp
	$(CXX) -c $(CPPFLAGS) -o $@  $<

# Rule for genertaing main.out file 
$(TARGET).out :$(OBJS)	
	$(CXX) -o $@ $(CPPFLAGS) $^
	
-include $(DEPS)  

.PHONY: all
all : $(TARGET).out

.PHONY: run
run : all
	./$(TARGET).out sample-program.tiny

.PHONY: clean
clean : 
	rm -rf $(OBJS) $(DEPS) $(TARGET).out