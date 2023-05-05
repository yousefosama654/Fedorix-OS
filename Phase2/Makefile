build:
	@if [ ! -d "./bin" ]; then mkdir "./bin"; fi
	@gcc ./src/process_generator.c -o ./bin/process_generator.out
	@gcc ./src/clk.c -o ./bin/clk.out
	@gcc ./src/scheduler.c -o ./bin/scheduler.out -lm
	@gcc ./src/process.c -o ./bin/process.out
	@gcc ./src/test_generator.c -o ./bin/test_generator.out
clean:
	@rm -f ./bin/*.out  ./Log/processes.txt ./Log/memory.log ./Log/Scheduler.log  ./Log/Scheduler.perf
test:
	@./bin/test_generator.out

# run: @./bin/process_generator.out

cleanlog:
	@rm -f ./Log/memory.log ./Log/Scheduler.log  ./Log/Scheduler.perf

all: clean build test 
#   all: cleanlog build 