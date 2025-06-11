#ifndef _VECTOR_H_
#define _VECTOR_H_


#include <stdint.h>
#include <stdbool.h>

#ifndef RAM_TYPE_VECTOR
#define RAM_TYPE_VECTOR   RAM_TYPE_DATA_ON
#endif

typedef void *VECTOR_ELE;


typedef struct VECTOR_STRUCT *VECTOR;
typedef bool (*V_PREDICATE)(VECTOR_ELE element, void *client_data);
typedef bool (*V_MAPPER)(VECTOR_ELE element, void *client_data);
typedef int32_t (*V_CMP)(VECTOR_ELE elementA, VECTOR_ELE elementB);



struct VECTOR_ITERATOR_STRUCT
{
    uint8_t idx;
    uint8_t size;
    V_CMP cmp;
    VECTOR_ELE array[];
};
typedef struct VECTOR_ITERATOR_STRUCT *VECTOR_ITERATOR;



VECTOR vector_create(uint32_t vector_size);
bool vector_delete(VECTOR vector);
uint32_t vector_depth(VECTOR vector);
bool vector_add(VECTOR vector, VECTOR_ELE element);
bool vector_remove(VECTOR vector, VECTOR_ELE element);
VECTOR_ELE vector_search(VECTOR vector, V_PREDICATE predicate, void *client_data);
bool vector_mapping(VECTOR vector, V_MAPPER mapper, void *client_data);


VECTOR_ITERATOR vector_iterator_create(VECTOR vector);
void vector_iterator_delete(VECTOR_ITERATOR iterator);
bool vector_iterator_step(VECTOR_ITERATOR iterator, VECTOR_ELE *p_ele);
#endif

