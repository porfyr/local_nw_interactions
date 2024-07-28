all:
	cd server && make
	cd client && make

clear:
	cd server && make clear
	cd client && make clear

run:
	@echo "This command works only in directories server & client or make run-<server/client>"

run-server:
	cd server && make run

run-client:
	cd client && make run

#test:
#	./$(TARGET) # Припустимо, що my_program - це програма, яка потребує запуску для тестування

help:
	@echo "Available targets:"
	@echo "  all        	: Compile the project"
	@echo "  clear      	: Remove object files and executable"
#	@echo "  testing       	: Run tests"
