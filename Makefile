obj=list.o

main:$(obj)
	gcc -o main $(obj)

$(obj):%.o:%.c
	gcc -c $< -o $@

.PHONY:clean

clean:
	rm -f $(obj) main