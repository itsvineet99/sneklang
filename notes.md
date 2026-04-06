### sneklang object

- we can create sneklang object.
	- this is a custom object that we have written 
	- it has kind and data
	- data is actual value
		- data is union with different types
		- union because we need to store different types here
	- kind is "what kind of data this object can store?"
	- kind is enum, cause we need only one value out of constants 
	- kind: 
		- INTEGER: it is stored as int in data
		- FLOAT: stored as float in data
		- STRING: stored char \*array
		- VECTOR3: custom snek_vector_t type
		- ARRAY: custom snek_array_t type 


###  snek_array_t

- this is a struct which stores 2 values 
- one is size (a int that tells how many elements we have in array)
- other is elements
	- here we create a pointer \*\*elements of type snek_object_t
	- this pointer points to a location which has a pointer in it and that pointer points to actual data / value
	- \*\*elements  is pointer -> pointer -> value
	- when we elements[0] we are on 2nd level pointers i.e elements[0] is equal to doing \*elements which basically dereferencing \*\*elements one time.
	- so elements[0] returns pointer to the original object then we can dereference our pointer one more time to access the values or we can use `->` keyword.

we use given line to declare elements:

```c
snek_object_t **elements = calloc(size, sizeof(snek_object_t *));
```

here calloc returns memory address to the first byte of the contiguous memory it allocates. this memory allocation happens depending on size and sizeof(). the sizeof() returns the amount of memory needed for one object and and size is how many of those objects we need to store.

so here `sizeof(snek_object_t *)` will return the amount of memory need to store the pointer of object and not the object itself. cause here we are storing pointers to values and not the actual value.

so calloc() will allocate memory needed to store the pointer of our object and will return the memory address of the first byte.

so basically `**elements` is storing address of the first byte of the memory allocated by calloc and this memory contains list of different pointers. so `**elements` is array of pointers.

we can do elements[0] cause elements is a pointer to first element of array.

double pointer is need cause we are storing here pointers. we need a pointer to store pointers hence double pointer.

### sneklang vector3

this stores 3 sneklang object in it.

its a struct where we store values of 3 neklang object basically.

### creating actual values

first write a function to create new sneklang object

then for each kind we have different function to set its kind value and data value inside it.

general structure:
- you use new sneklang object function to create new object
- check if obj is null, if it is then return null
- then set kind to its kind and data to its data

differences:
- for string we can't set value of one string variable to another string variable if we are using array method to create string `char str[10] = "hello";`
- but if we are using pointer method like `char *str = "hello";` we can copy the value of string
- but setting up this value directly to our objects value can cause memory issue like Spooky Action at a Distance (Mutation) 
- so we first allocate some heap memory to our string objects data element using malloc and then we sue strcpy to copy the value to this new memory 

- difference for array is we have seen already and vector3 doesn't have much its similar.

### get and set method for arrays

get method for array is simple we just add few checks before accessing the value we add checks like check if array is null and if we are calling get on array or some other type and if index is out of bond. once all this checks are satisfied we just simply access the value with given syntax

```c
return snek_obj->data.v_array.elements[index];
```

set is also similar. we do the same checks but only difference is now when we update the value on some index where some other value already existed we need to decrease its reference count as this value will get replaced by some other value and we don't need this value no more.

### length method

this returns length of each method.
- for integer and float its 1
- for vector3 its 3
- for array we use its size element
- for string we use strlen() method to count it

### add method

this method we use to addition between types and sometimes with other types.

- we take two arguments a and b in function.
- we apply switch for a's kind.
- for each type then we apply switch on b's kind
	- for some types like int and float we allow them to add with each other but we do some type casting in this case
	- for string, vector3 and array we don't allow them to add to different types

- for string we allocate memory to new variable and the length of memory is addition of the size of both the strings plus 1 for null terminator
- first copy a to new string using strcpy
- then concat b to new string using strcat

- for vectors we just add x to x, y to y and z to z

- for array we concat both arrays by creating new array objecting and then using for loop to set values to the indexes. first a then b.

- in the end after each kind we create new object of that kind before returning this new added object.

### refcount inc, dec and free

refcount_inc simply increments refcount element of our object

refcount_dec simply decrements refcount but when refcount reaches to zero it calls refcount_free.

free simply uses free() remove object from memory but we have to deal with the data field of our custom types too, like freeing v_string value or x,y,z in vector3 etc.

for vector and array we don't directly free but we do refcount_dec on them cause they are not values but objects so their refcount must reach zero before being freed.
