INC      = $(GUROBI_HOME)/include/
CPP      = g++ -std=c++11
CARGS    = -m64 -g -O3
CPPLIB   = -L$(GUROBI_HOME)/lib -lgurobi_c++ -lgurobi95

#### diretorios com os source files e com os objs files
SRCDIR = src
OBJDIR = obj
#############################

#### lista de todos os srcs e todos os objs
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCS))
#############################

exe.out: $(OBJS) #$(MAINOBJ) $(CUTOBJ)
	@echo  "\033[31m \nLinking all objects files: \033[0m"
	$(CPP) $(CARGS) $(OBJS) -o $@ -I$(INC) $(CPPLIB) -lm -Wall -ggdb3 -lpthread 

#inclui os arquivos de dependencias
-include $(OBJS:.o=.d)

#regra para cada arquivo objeto: compila e gera o arquivo de dependencias do arquivo objeto
#cada arquivo objeto depende do .c e dos headers (informacao dos header esta no arquivo de dependencias gerado pelo compiler)
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo  "\033[31m \nCompiling $<: \033[0m"
	$(CPP) -c $< -o $@ -I$(INC) $(CPPLIB) -lm -Wall -ggdb3	
	@echo  "\033[32m \ncreating $< dependency file: \033[0m"
	$(CPP) -std=c++0x  -I$(INC) $(CPPLIB) -MM $< > $(basename $@).d
	@mv -f $(basename $@).d $(basename $@).d.tmp #proximas tres linhas colocam o diretorio no arquivo de dependencias (g++ nao coloca, surprisingly!)
	@sed -e 's|.*:|$(basename $@).o:|' < $(basename $@).d.tmp > $(basename $@).d
	@rm -f $(basename $@).d.tmp

#delete objetos e arquivos de dependencia
clean:
	@echo "\033[31mcleaning obj directory and binary\033[0m"
	@rm -f $(OBJDIR)/*.o $(OBJDIR)/*.d
	@rm -f exe

rebuild: clean exe
