obj := binary_tree.o main.o Case011.o Case013.o single_list.o

main:$(obj)
	gcc -o main $(obj)

$(obj):%.o:%.c
	gcc -g -c $< -o $@

.PHONY:clean

clean:
	rm -f $(obj) main
