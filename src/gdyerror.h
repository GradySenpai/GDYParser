/*
* This is going to be the starting point for the transition to a more c style core to the statment class.
* In the end result, all of this should be abstracted away and class users should not ever see it; it will
* be completely behind the scene.
*/
#pragma once
#include<cstdio>
#include<cstdlib>

typedef void** ALLOC_BUFFER;

//if no custom error is defined than this is the error function called
static inline void _global_error(const char* error) noexcept {
	printf("[ERROR]"); printf(error);
	exit(-1);
}

#ifndef _GDY_NO_CUSTOM_ERR
#define _GDY_VALIDATE(cond, err) if (!(cond))
#endif
#ifndef _GDY_DEFAULT_SAFE_BUFFER_LEN
#define _GDY_DEFAULT_SAFE_BUFFER_LEN 8 * sizeof(void*)
#endif
#ifndef _GDY_LOCAL_BUFFER_ONLY
ALLOC_BUFFER g_alloc_buffer;
#define GDY_MALLOC_SETUP _gdy_safe_malloc_setup(&g_alloc_buffer)
#define GDY_CREATE_ARR(type, name, size) type* name = (type*)_gdy_safe_malloc(&g_alloc_buffer, size * sizeof(type))
#define GDY_MALLOC_CLEANUP _gdy_safe_cleanup(&g_alloc_buffer); 
#define GDY_REALLOC(type, arr, size) _gdy_safe_realloc(&g_alloc_buffer, arr, size * sizeof(type))
#endif
#define _GDY_SAFE_BUFFER_LEN(buffer) *((unsigned int*)(buffer))
#define _GDY_SAFE_BUFFER_CAP(buffer) *((unsigned int*)(buffer) + 1)
#define _GDY_SAFE_BUFFER_AT(buffer, index) (buffer)[index + 2]

//safe allocation functions

//create safe alloc buffer
static inline int _gdy_safe_malloc_setup(ALLOC_BUFFER* _alloc_buffer) noexcept {
	*_alloc_buffer = (ALLOC_BUFFER)malloc(sizeof(size_t) + _GDY_DEFAULT_SAFE_BUFFER_LEN);
	if (*_alloc_buffer == NULL) return 1;
	_GDY_SAFE_BUFFER_LEN(*_alloc_buffer) = 0;
	_GDY_SAFE_BUFFER_CAP(*_alloc_buffer) = _GDY_DEFAULT_SAFE_BUFFER_LEN;
	return 0;
}

static inline void* _gdy_safe_malloc(ALLOC_BUFFER* _alloc_buffer, size_t len) noexcept {
	if (_GDY_SAFE_BUFFER_LEN(*_alloc_buffer) >= _GDY_SAFE_BUFFER_CAP(*_alloc_buffer)) {
		ALLOC_BUFFER _new_buffer =
			(ALLOC_BUFFER)realloc(_alloc_buffer, _GDY_SAFE_BUFFER_CAP(*_alloc_buffer) * 2);
		if (_new_buffer == NULL) { free(*_alloc_buffer); return NULL; }
		*_alloc_buffer = _new_buffer;
	} 
	void* ret = malloc(len);
	if (ret == NULL) return NULL;
	_GDY_SAFE_BUFFER_AT(*_alloc_buffer, _GDY_SAFE_BUFFER_LEN(*_alloc_buffer)) = ret;
	return ret;
}

static inline void* _gdy_safe_realloc(ALLOC_BUFFER* _alloc_buffer, void* _old, size_t _new_size) noexcept {
	void* ret = realloc(_old, _new_size);
	if (ret == NULL) return NULL;
	int i = 0;
	while (_GDY_SAFE_BUFFER_AT(*_alloc_buffer, i) != _old) i++;
	_GDY_SAFE_BUFFER_AT(*_alloc_buffer, i) = ret;
	return ret;
}

static inline void _gdy_safe_free(ALLOC_BUFFER* _alloc_buffer, void* _to_free) noexcept {
	int i = 0;
	while (_GDY_SAFE_BUFFER_AT(*_alloc_buffer, i) != _to_free) i++;
	free(_GDY_SAFE_BUFFER_AT(*_alloc_buffer, i));
	_GDY_SAFE_BUFFER_AT(*_alloc_buffer, i) = NULL;
}

static inline void _gdy_safe_cleanup(ALLOC_BUFFER* _alloc_buffer) noexcept {
	int i = 0;
	while (i != _GDY_SAFE_BUFFER_LEN(*_alloc_buffer)) {
		free(_GDY_SAFE_BUFFER_AT(*_alloc_buffer, i));
		i++;
	} free(*_alloc_buffer);
}