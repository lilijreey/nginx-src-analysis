
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_LIST_H_INCLUDED_
#define _NGX_LIST_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>

part
+-----+-------+------+      -+-----+-------+------+     -+-----+-------+------+    
| elts| nelts | next |---->  | elts| nelts | next |----> | elts| nelts | next |
+-----+-------+------+      -+-----+-------+------+     -+-----+-------+------+ 
  \                           \                            \                       
   \                           \                            \
    +----------+                +----------+                 +----------+
    | buf      |                | buf      |                 | buf      |
    +----------+                +----------+                 +----------+

  
typedef struct ngx_list_part_s  ngx_list_part_t;

//一个元素桶， 
struct ngx_list_part_s {
    void             *elts; //可用空间首地址
    ngx_uint_t        nelts; //part 当前存了几个元素
    ngx_list_part_t  *next;
};


typedef struct {
    ngx_list_part_t  *last;    //最后一个part
    ngx_list_part_t   part;    //第一个part
    size_t            size;    //每个part的大小
    ngx_uint_t        nalloc;  //每个part中能存元素的个数
    ngx_pool_t       *pool;
} ngx_list_t;


ngx_list_t *ngx_list_create(ngx_pool_t *pool, ngx_uint_t n, size_t size);

// n 每个part可以存储元素的数量， size每个元素的大小
static ngx_inline ngx_int_t
ngx_list_init(ngx_list_t *list, ngx_pool_t *pool, ngx_uint_t n, size_t size)
{
    //每回分配一个part的空间
    list->part.elts = ngx_palloc(pool, n * size);
    if (list->part.elts == NULL) {
        return NGX_ERROR;
    }

    list->part.nelts = 0;
    list->part.next = NULL;
    list->last = &list->part;
    list->size = size;
    list->nalloc = n;
    list->pool = pool;

    return NGX_OK;
}


/*
 *
 *  the iteration through the list:
 *
 *  part = &list.part;
 *  data = part->elts;
 *
 *  for (i = 0 ;; i++) {
 *
 *      if (i >= part->nelts) {
 *          if (part->next == NULL) {
 *              break;
 *          }
 *
 *          part = part->next;
 *          data = part->elts;
 *          i = 0;
 *      }
 *
 *      ...  data[i] ...
 *
 *  }
 */


void *ngx_list_push(ngx_list_t *list);


#endif /* _NGX_LIST_H_INCLUDED_ */
