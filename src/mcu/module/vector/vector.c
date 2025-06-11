#include <string.h>

#include "vector.h"
#include "os_mem.h"
#include "trace.h"


struct VECTOR_STRUCT
{
    uint32_t depth;
    uint32_t array_size;
    uint32_t capacity;
    VECTOR_ELE *array;
};


typedef enum
{
    VECTOR_UNION,
    VECTOR_INTERSECTION,
    VECTOR_DIFFERENCE,
} VECTOR_SET_OP_TYPE;


typedef struct
{
    VECTOR vectorA;
    VECTOR vectorB;
    V_CMP cmp;
}   VECTOR_SET_OP_IN_PARAM;


typedef struct
{
    VECTOR vectorC;
    VECTOR_ITERATOR iteratorA;
    VECTOR_ITERATOR iteratorB;
} VECTOR_SET_OP_OUT_PARAM;


typedef enum
{
    INSERT_SORT,
    MERGE_SORT,
    MAX_SORT,
} VECTOR_SORT_TYPE;


typedef void (*VECTOR_ITERATOR_SORT)(VECTOR_ITERATOR iterator);
static void vector_iterator_insert_sort(VECTOR_ITERATOR iterator);
static void vector_iterator_merge_sort(VECTOR_ITERATOR iterator);


struct
{
    VECTOR_ITERATOR_SORT sort_methods[MAX_SORT];
    V_CMP cmp;
} vector_mgr = {.sort_methods = {[INSERT_SORT] = vector_iterator_insert_sort, [MERGE_SORT] = vector_iterator_merge_sort},
                .cmp = NULL
               };


static bool vector_resize(VECTOR vector, uint32_t new_size)
{
    VECTOR_ELE *new_array = NULL;
    new_array = os_mem_zalloc(RAM_TYPE_VECTOR, new_size * sizeof(VECTOR_ELE));
    if (new_array != NULL)
    {
        if (new_size >= vector->array_size)
        {
            memcpy(new_array, vector->array, vector->array_size * sizeof(VECTOR_ELE));
        }
        else
        {
            uint32_t new_array_idx = 0;
            uint32_t old_idx = 0;

            for (old_idx = 0; old_idx < vector->array_size && new_array_idx < new_size; old_idx++)
            {
                if (vector->array[old_idx] != NULL)
                {
                    new_array[new_array_idx] = vector->array[old_idx];
                    new_array_idx++;
                }
            }

            if (old_idx != vector->array_size)
            {
                APP_PRINT_ERROR0("vector_resize: cannot walk through old array");
                os_mem_free(new_array);
                return false;
            }
        }
        os_mem_free(vector->array);

        vector->array_size = new_size;
        vector->array = new_array;
    }

    return true;
}


VECTOR vector_create(uint32_t capacity)
{
    VECTOR vector = NULL;

    if (capacity == 0)
    {
        APP_PRINT_ERROR0("vector_create: capacity should greater than 0");
        return NULL;
    }

    vector = os_mem_zalloc(RAM_TYPE_VECTOR, sizeof(struct VECTOR_STRUCT));
    if (vector == NULL)
    {
        goto FREE_VECTOR;
    }

    vector->array = os_mem_zalloc(RAM_TYPE_VECTOR, 1 * sizeof(VECTOR_ELE));
    if (vector->array == NULL)
    {
        goto FREE_ARRAY;
    }

    vector->depth = 0;
    vector->capacity = capacity;
    vector->array_size = 1;
    return vector;

FREE_ARRAY:
    os_mem_free(vector->array);

FREE_VECTOR:
    os_mem_free(vector);

    return NULL;
}



bool vector_delete(VECTOR vector)
{
    if (vector == NULL)
    {
        APP_PRINT_ERROR0("vector_delete: vector is NULL");
        return false;
    }

    if (vector->array != NULL)
    {
        os_mem_free(vector->array);
    }

    os_mem_free(vector);

    return true;
}


uint32_t vector_depth(VECTOR vector)
{
    if (vector == NULL)
    {
        APP_PRINT_ERROR0("vector_depth: vector is NULL");
        return false;
    }

    return vector->depth;
}


bool vector_add(VECTOR vector, VECTOR_ELE element)
{
    uint32_t array_size = 0;
    uint32_t i = 0;
    VECTOR_ELE *array = NULL;

    if (vector == NULL)
    {
        APP_PRINT_ERROR0("vector_add: vector is NULL");
        return false;
    }

    if (vector->depth == vector->capacity)
    {
        APP_PRINT_ERROR0("vector_add: vector is full");
        return false;
    }

    if (vector->depth == vector->array_size)
    {
        vector_resize(vector, vector->array_size * 2);
    }

    array_size = vector->array_size;
    array = vector->array;

    if (element == NULL)
    {
        APP_PRINT_WARN0("vector_add: element is NULL");
    }

    for (i = 0; i < array_size; i++)
    {
        if (array[i] == NULL)
        {
            array[i] = element;
            vector->depth += 1;
            //APP_PRINT_TRACE1("vector_add: pos %d", i);

            return true;
        }
    }

    APP_PRINT_ERROR0("vector_add: cannot found an empty pos");

    return false;
}


bool vector_remove(VECTOR vector, VECTOR_ELE element)
{
    uint32_t array_size = 0;
    uint32_t i = 0;
    VECTOR_ELE *array = NULL;

    if (vector == NULL)
    {
        APP_PRINT_ERROR0("vector_remove: vector is NULL");
        return false;
    }

    if (vector->array == NULL)
    {
        APP_PRINT_ERROR0("vector_remove: vector array is NULL");
        return false;
    }

    if (vector->depth == 0)
    {
        APP_PRINT_ERROR0("vector_remove: vector is empty");
        return false;
    }

    if (element == NULL)
    {
        APP_PRINT_WARN0("vector_remove: element is NULL");
    }

    array = vector->array;
    array_size = vector->array_size;

    for (i = 0; i < array_size; i++)
    {
        if (array[i] == element)
        {
            array[i] = NULL;
            vector->depth -= 1;

            if (vector->depth > 0 && vector->depth == (vector->array_size / 4))
            {
                vector_resize(vector, vector->array_size / 2);
            }

            return true;
        }
    }

    return false;
}





VECTOR_ELE vector_search(VECTOR vector, V_PREDICATE predicate, void *client_data)
{
    uint32_t size = 0;
    uint32_t i = 0;
    uint32_t depth = 0;
    VECTOR_ELE *array = NULL;
    VECTOR_ELE element = NULL;

    if (vector == NULL)
    {
        APP_PRINT_ERROR0("vector_search: vector is NULL");
        return NULL;
    }

    size = vector->array_size;
    array = vector->array;
    depth = vector->depth;

    if (vector->depth == 0)
    {
        APP_PRINT_ERROR0("vector_search: vector is empty");
        return NULL;
    }

    if (predicate == NULL)
    {
        APP_PRINT_ERROR0("vector_search: predicate is NULL");
        return NULL;
    }

    for (i = 0; i < size && depth > 0; i++) // ({if...}) is a statement expression
    {
        element = array[i];

        if (element != NULL)
        {
            if (predicate(element, client_data) == true)
            {
                //APP_PRINT_TRACE1("vector_search: pos %d", i);
                return element;
            }

            depth--;
        }
    }

    APP_PRINT_ERROR0("vector_search: cannot found this element");

    return NULL;
}


bool vector_mapping(VECTOR vector, V_MAPPER mapper, void *client_data)
{
    uint32_t size = 0;
    uint32_t i = 0;
    VECTOR_ELE *array = NULL;
    uint32_t depth = 0;
    VECTOR_ELE element = NULL;

    if (vector == NULL)
    {
        APP_PRINT_ERROR0("vector_mapping: vector is NULL");
        return false;
    }

    size = vector->array_size;
    array = vector->array;
    depth = vector->depth;

    if (vector->depth == 0)
    {
        APP_PRINT_ERROR0("vector_mapping: vector is empty");
        return false;
    }

    if (mapper == NULL)
    {
        APP_PRINT_ERROR0("vector_mapping: mapper is NULL");
        return false;
    }

    for (i = 0; i < size && depth > 0; i++)
    {
        element = array[i];

        if (element != NULL)
        {
            mapper(element, client_data);

            depth--;
        }
    }

    return true;
}


VECTOR_ITERATOR vector_iterator_create(VECTOR vector)
{
    VECTOR_ITERATOR iterator = NULL;
    uint32_t iterator_size = vector->depth;
    uint32_t vector_size = vector->array_size;
    uint32_t i = 0;
    uint32_t idx = 0;

    iterator = os_mem_zalloc(RAM_TYPE_VECTOR,
                             sizeof(struct VECTOR_ITERATOR_STRUCT) + iterator_size * sizeof(VECTOR_ELE));
    if (iterator == NULL)
    {
        APP_PRINT_TRACE0("vector_iterator_create: malloc failed");
        return NULL;
    }

    iterator->idx = 0;
    iterator->size = iterator_size;

    for (i = 0; i < vector_size && idx < iterator_size; i++)
    {
        if (vector->array[i] != NULL)
        {
            iterator->array[idx] = vector->array[i];
            idx++;
        }
    }

    return iterator;
}


void vector_iterator_delete(VECTOR_ITERATOR iterator)
{
    if (iterator != NULL)
    {
        os_mem_free(iterator);
    }
}


bool vector_iterator_step(VECTOR_ITERATOR iterator, VECTOR_ELE *p_ele)
{
    if (iterator->idx == iterator->size)
    {
        *p_ele = NULL;
        iterator->idx = 0;
        return false;
    }

    *p_ele = iterator->array[iterator->idx];
    iterator->idx++;

    return true;
}


static void vector_update_by_iterator(VECTOR vector, VECTOR_ITERATOR iterator)
{
    memset(vector->array, NULL, vector->array_size * sizeof(vector->array[0]));
    memcpy(vector->array, iterator->array, iterator->size * sizeof(vector->array[0]));
}


static void vector_merge(VECTOR_ELE *ori_array, VECTOR_ELE *tmp_array, uint32_t lpos, uint32_t rpos,
                         uint32_t rightend)
{
    uint32_t i = 0, leftend = 0, N = 0, tmp_pos = 0;

    leftend = rpos - 1;
    tmp_pos = lpos;
    N = rightend - lpos + 1;

    while (lpos <= leftend && rpos <= rightend)
    {
        if (vector_mgr.cmp(ori_array[lpos], ori_array[rpos]) < 0)
        {
            tmp_array[tmp_pos++] = ori_array[lpos++];
        }
        else
        {
            tmp_array[tmp_pos++] = ori_array[rpos++];
        }
    }

    while (lpos <= leftend)
    {
        tmp_array[tmp_pos++] = ori_array[lpos++];
    }

    while (rpos <= rightend)
    {
        tmp_array[tmp_pos++] = ori_array[rpos++];
    }

    for (i = 0; i < N; i++, rightend--)
    {
        ori_array[rightend] = tmp_array[rightend];
    }
}


static void vector_msort(VECTOR_ELE *ori_array, VECTOR_ELE *tmp_array, uint32_t left,
                         uint32_t right)
{
    uint32_t center = 0;

    if (left < right)
    {
        center = (left + right) / 2;

        vector_msort(ori_array, tmp_array, left, center);
        vector_msort(ori_array, tmp_array, center + 1, right);
        vector_merge(ori_array, tmp_array, left, center + 1, right);
    }
}


static void vector_iterator_merge_sort(VECTOR_ITERATOR iterator)
{
    VECTOR_ELE *tmp_array = NULL;

    tmp_array = os_mem_zalloc(RAM_TYPE_VECTOR, iterator->size * sizeof(*tmp_array));
    if (tmp_array != NULL)
    {
        vector_msort(iterator->array, tmp_array, 0, iterator->size - 1);
    }
    else
    {
        APP_PRINT_ERROR0("vector_iterator_merge_sort: malloc failed");
    }
}


static void vector_isort(VECTOR_ELE *array, uint32_t N)
{
    uint32_t j = 0, p = 0;
    VECTOR_ELE tmp = NULL;

    for (p = 1; p < N; p++)
    {
        tmp = array[p];
        for (j = p; j > 0 && array[j - 1] > tmp; j--)
        {
            array[j] = array[j - 1];
        }
        array[j] = tmp;
    }
}


static void vector_iterator_insert_sort(VECTOR_ITERATOR iterator)
{
    vector_isort(iterator->array, iterator->size);
}


static int32_t vector_iterator_binary_search(VECTOR_ELE target_ele, VECTOR_ELE *ele_array,
                                             int32_t low, int32_t high)
{
    int32_t mid = 0, cmp_res = 0;

    if (low > high) { return -1; }

    mid = (low + high) / 2;
    cmp_res = vector_mgr.cmp(target_ele, ele_array[mid]);

    if (cmp_res == 0)
    {
        return mid;
    }
    else if (cmp_res < 0)
    {
        return vector_iterator_binary_search(target_ele, ele_array, low, mid - 1);
    }
    else
    {
        return vector_iterator_binary_search(target_ele, ele_array, mid + 1, high);
    }
}


bool vector_set_op_prepare(VECTOR_SET_OP_TYPE op, VECTOR_SET_OP_IN_PARAM *p_in_param,
                           VECTOR_SET_OP_OUT_PARAM *p_out_param)
{
    VECTOR_ITERATOR iteratorA = NULL, iteratorB = NULL;
    VECTOR vectorC = NULL;
    uint32_t new_vector_size = 0;

    if (p_in_param->cmp == NULL)
    {
        return false;
    }

    iteratorA = vector_iterator_create(p_in_param->vectorA);
    iteratorB = vector_iterator_create(p_in_param->vectorB);


    vector_mgr.cmp = p_in_param->cmp;

    VECTOR_SORT_TYPE sort_type = MAX_SORT;
    if (iteratorA->size < 20 && iteratorB->size < 20)
    {
        sort_type = INSERT_SORT;
    }
    else
    {
        sort_type = MERGE_SORT;
    }

    vector_mgr.sort_methods[sort_type](iteratorA);
    vector_mgr.sort_methods[sort_type](iteratorB);

    switch (op)
    {
    case VECTOR_UNION:
        new_vector_size = iteratorA->size + iteratorB->size;
        break;

    case VECTOR_INTERSECTION:
        new_vector_size = iteratorA->size < iteratorB->size ? iteratorA->size : iteratorB->size;
        break;

    case VECTOR_DIFFERENCE:
        new_vector_size = iteratorA->size;
        break;

    default:
        break;
    }

    vectorC = vector_create(new_vector_size);

    p_out_param->iteratorA = iteratorA;
    p_out_param->iteratorB = iteratorB;
    p_out_param->vectorC = vectorC;
    vector_update_by_iterator(p_in_param->vectorA, iteratorA);
    vector_update_by_iterator(p_in_param->vectorB, iteratorB);


    return true;
}


static bool vector_set_op_end(VECTOR_SET_OP_OUT_PARAM *out_param)
{
    if (out_param->iteratorA != NULL)
    {
        os_mem_free(out_param->iteratorA);
    }

    if (out_param->iteratorB != NULL)
    {
        os_mem_free(out_param->iteratorB);
    }

    vector_mgr.cmp = NULL;

    return true;
}


VECTOR vector_union(VECTOR vectorA, VECTOR vectorB, V_CMP cmp)
{
    VECTOR_SET_OP_IN_PARAM in_param = {0};
    VECTOR_SET_OP_OUT_PARAM out_param = {0};
    VECTOR_ELE ele = NULL;
    bool ret = false;

    in_param.cmp = cmp;
    in_param.vectorA = vectorA;
    in_param.vectorB = vectorB;

    ret = vector_set_op_prepare(VECTOR_UNION, &in_param, &out_param);
    if (ret == false) { return NULL; }

    while (vector_iterator_step(out_param.iteratorA, &ele))
    {
        vector_add(out_param.vectorC, ele);
    }

    while (vector_iterator_step(out_param.iteratorB, &ele))
    {
        if (vector_iterator_binary_search(ele, out_param.iteratorA->array, 0,
                                          out_param.iteratorA->size - 1) == -1)
        {
            vector_add(out_param.vectorC, ele);
        }
    }

    vector_set_op_end(&out_param);

    return out_param.vectorC;
}


static void __vector_intersection(VECTOR vector, VECTOR_ITERATOR iterator_base,
                                  VECTOR_ITERATOR iterator_compare)
{
    VECTOR_ELE ele = NULL;

    while (vector_iterator_step(iterator_base, &ele))
    {
        if (vector_iterator_binary_search(ele, iterator_compare->array, 0, iterator_compare->size - 1) >= 0)
        {
            vector_add(vector, ele);
        }
    }
}


VECTOR vector_intersection(VECTOR vectorA, VECTOR vectorB, V_CMP cmp)
{
    VECTOR_SET_OP_IN_PARAM in_param = {0};
    VECTOR_SET_OP_OUT_PARAM out_param = {0};
    bool ret = false;

    in_param.cmp = cmp;
    in_param.vectorA = vectorA;
    in_param.vectorB = vectorB;

    ret = vector_set_op_prepare(VECTOR_INTERSECTION, &in_param, &out_param);
    if (ret == false) { return NULL; }

    if (out_param.iteratorA->size <= out_param.iteratorB->size)
    {
        __vector_intersection(out_param.vectorC, out_param.iteratorA, out_param.iteratorB);
    }
    else
    {
        __vector_intersection(out_param.vectorC, out_param.iteratorB, out_param.iteratorA);
    }

    vector_set_op_end(&out_param);

    return out_param.vectorC;
}


VECTOR vector_difference(VECTOR vectorA, VECTOR vectorB, V_CMP cmp)
{
    VECTOR_SET_OP_IN_PARAM in_param = {0};
    VECTOR_SET_OP_OUT_PARAM out_param = {0};
    VECTOR_ELE ele = NULL;
    bool ret = false;

    in_param.cmp = cmp;
    in_param.vectorA = vectorA;
    in_param.vectorB = vectorB;


    ret = vector_set_op_prepare(VECTOR_DIFFERENCE, &in_param, &out_param);
    if (ret == false) { return NULL; }

    while (vector_iterator_step(out_param.iteratorA, &ele))
    {
        if (vector_iterator_binary_search(ele, out_param.iteratorB->array, 0,
                                          out_param.iteratorB->size - 1) == -1)
        {
            vector_add(out_param.vectorC, ele);
        }
    }

    vector_set_op_end(&out_param);

    return out_param.vectorC;
}


static int32_t test_cmp(uint32_t *p_a, uint32_t *p_b)
{
    if (*p_a < *p_b)
    {
        return -1;
    }
    else if (*p_a > *p_b)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


bool vector_set_test(void)
{
    uint32_t a[6] = {7, 8, 9, 10, 32, 64};
    uint32_t b[5] = {32, 7, 1, 2, 3};
    VECTOR vectorA = NULL;
    VECTOR vectorB = NULL;
    VECTOR vectorC = NULL;
    VECTOR_ITERATOR iteratorC = NULL;
    uint32_t *temp = 0;

    vectorA = vector_create(6);
    for (uint32_t i = 0; i < 6; i++)
    {
        vector_add(vectorA, &a[i]);
    }

    vectorB = vector_create(5);
    for (uint32_t i = 0; i < 5; i++)
    {
        vector_add(vectorB, &b[i]);
    }

    vectorC = vector_union(vectorA, vectorB, (V_CMP)test_cmp);
    iteratorC = vector_iterator_create(vectorC);

    while (vector_iterator_step(iteratorC, (VECTOR_ELE *)&temp))
    {
        APP_PRINT_TRACE1("%d", *temp);
    }

    return true;
}


