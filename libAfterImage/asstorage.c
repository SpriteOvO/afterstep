/* This file contains code for memopry management for image data    */
/********************************************************************/
/* Copyright (c) 2004 Sasha Vasko <sasha at aftercode.net>          */
/********************************************************************/
/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#undef LOCAL_DEBUG
#undef DEBUG_COMPRESS
#undef DO_CLOCKING

#ifdef _WIN32
#include "win32/config.h"
#else
#include "config.h"
#endif

#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <memory.h>

#ifndef HAVE_ZLIB_H
#include "zlib/zlib.h"
#else
#include <zlib.h>
#endif

#ifdef _WIN32
# include "win32/afterbase.h"
#else
# include "afterbase.h"
#endif

#include "asstorage.h"

/* default storage : */

ASStorage *_as_default_storage = NULL ;


/************************************************************************/
/* Private Functions : 													*/
/************************************************************************/

#define StorageID2BlockIdx(id)    	(((((CARD32)(id))>>14)&0x0003FFFF)-1)
#define StorageID2SlotIdx(id)    	 ((((CARD32)(id))&0x00003FFF)-1)

static size_t UsedMemory = 0 ;
static size_t UncompressedSize = 0, CompressedSize = 0 ;

static inline ASStorageID 
make_asstorage_id( int block_id, int slot_id )
{
	ASStorageID id = 0 ;
	if( block_id > 0 && block_id < (0x01<<18)&& slot_id > 0 && slot_id < (0x01<<14)) 
		id = ((CARD32)block_id<<14)|(CARD32)slot_id ;
	return id;
}

static int 
rlediff_compress_bitmap( CARD8 *buffer,  CARD8* data, int size, CARD8 bitmap_threshold )
{
	int i = 0 ; 
	CARD8 last_val = 0 ;
	int comp_size = 0 ;
	
	while( i < size ) 
	{
		int count = 0 ;
		while( count < 255 	&& i < size ) 
		{
			if( (( data[i] > bitmap_threshold )?1:0) != last_val ) 	  
				break;
			++count ;
			++i ;
		}	 
		last_val = (last_val == 1)?0:1 ;
		buffer[comp_size++] = count ;
	}
			   
	return comp_size;	
}	 

static int 
rlediff_compress( CARD8 *buffer,  CARD8* data, int size )
{
	Bool sign[64] ;
	CARD8 diff[64], last_val = 0 ;
	int comp_size = 1 ;
	int i = 1;

	buffer[0] = last_val = data[0] ; 
#if defined(DEBUG_COMPRESS) && !defined(NO_DEBUG_OUTPUT)
 	fprintf(stderr, "first byte: 0x%2.2X \n", last_val );
#endif				
	while( i < size ) 
	{
		int run_step = 0;
		int run_size2 = 0;
		int d = (data[i] < last_val)?last_val-data[i]:data[i]-last_val ;
		
		if( d == 0 ) 
		{
			int zero_size = 0 ;  /* intentionally ! */ 
			while( ++i < size && zero_size < 127 ) 	 
			{	
				if( data[i] != last_val ) 
					break;
				++zero_size ;
			}
#if defined(DEBUG_COMPRESS) && !defined(NO_DEBUG_OUTPUT)
			fprintf( stderr, "comp_size = %d at line %d\n", comp_size, __LINE__ );
#endif
			if( comp_size + 1 > size )
				return 0; 
			buffer[comp_size] = RLE_ZERO_SIG | (zero_size&RLE_ZERO_LENGTH) ;
#if defined(DEBUG_COMPRESS) && !defined(NO_DEBUG_OUTPUT)
			fprintf(stderr, "in %d out %d: 0x%2.2X  - %d zeros\n", i, comp_size, buffer[comp_size], zero_size+1 );
#endif
			++comp_size ;
		}else if( d <= 8 )  
		{ /* see if we can pack everything into 2 or 4 bit string */
			do
			{
				if( data[i] == last_val	) 
					break;
				sign[run_step] = (data[i] > last_val )?0:1;
				diff[run_step] = sign[run_step]?last_val-data[i]:data[i]-last_val ;
				if( diff[run_step] > 8 ) 
					break;
				if( run_size2 == run_step ) 
				{	
					if( diff[run_step] > 2 )
					{
						if( run_size2 >= 4 )
							break;
					}else if( ++run_size2  >= 16 ) 
					{
						last_val = data[i] ;
						++i ;	  
						break; 
					}
				}
				
				++run_step ;
				last_val = data[i] ;
				++i ;
			}while( i < size && run_step < 64 ); 	
			if( run_step > run_size2 ) 
			{                  /* encoding as 4 bit values */
				int k = 0;
#if defined(DEBUG_COMPRESS) && !defined(NO_DEBUG_OUTPUT)
				fprintf( stderr, "comp_size = %d, run_step = %d at line %d\n", comp_size, run_step, __LINE__ );
#endif
				if( comp_size + 1 + run_step/2 > size )
					return 0; 

				buffer[comp_size] = RLE_NOZERO_SHORT_SIG | ((run_step-1)&RLE_NOZERO_SHORT_LENGTH) ;											   
#if defined(DEBUG_COMPRESS) && !defined(NO_DEBUG_OUTPUT)
				fprintf(stderr, "in %d out %d: 0x%2.2X  - %d 4bits things\n", i, comp_size, buffer[comp_size], run_step );
#endif				
				++comp_size;
				do
				{
					buffer[comp_size] = (sign[k]<<7)|((diff[k]-1)<<4) ;
					if( ++k < run_step )
					{	
						buffer[comp_size] |= (sign[k]<<3)|(diff[k]-1) ;
						++k ;
					}
#if defined(DEBUG_COMPRESS) && !defined(NO_DEBUG_OUTPUT)
					fprintf(stderr, "0x%2.2X ", buffer[comp_size] );
#endif
					++comp_size ;
				}while( k  < run_step );
			}else	 
			{					/* encoding as 2 bit values */
				int k = 0;
#if defined(DEBUG_COMPRESS) && !defined(NO_DEBUG_OUTPUT)
				fprintf( stderr, "comp_size = %d, run_step = %d at line %d\n", comp_size, run_size2, __LINE__ );
#endif
				if( comp_size + 1 + run_size2/4 > size )
					return 0; 

				buffer[comp_size] = RLE_NOZERO_LONG1_SIG | ((run_size2-1)&RLE_NOZERO_LONG_LENGTH) ;											   
#if defined(DEBUG_COMPRESS) && !defined(NO_DEBUG_OUTPUT)
				fprintf(stderr, "in %d out %d: 0x%2.2X  - %d 2bits things\n", i, comp_size, buffer[comp_size], run_size2 );
#endif				
				++comp_size;
				do
				{
					buffer[comp_size] = (sign[k]<<7)|((diff[k]-1)<<6) ;
					if( ++k < run_size2 )
					{	
						buffer[comp_size] |= (sign[k]<<5)|((diff[k]-1)<<4) ;
						if( ++k < run_size2 )
						{
							buffer[comp_size] |= (sign[k]<<3)|((diff[k]-1)<<2) ;
							if( ++k < run_size2 )
							{	
								buffer[comp_size] |= (sign[k]<<1)|(diff[k]-1) ;
								++k ;
							}
						}
					}
#if defined(DEBUG_COMPRESS) && !defined(NO_DEBUG_OUTPUT)
					fprintf(stderr, "0x%2.2X ", buffer[comp_size] );
#endif
					++comp_size ;
				}while( k  < run_size2 );
			}	 
		}else if( d <= 128 )  
		{                      /* 8 bit strings */
			int k = 0;
			do
			{
				if( data[i] == last_val	) 
					break;
				sign[run_step] = (data[i] > last_val )?0:1;
				diff[run_step] = sign[run_step]?last_val-data[i]:data[i]-last_val ;
				if( diff[run_step] > 128 || diff[run_step] <= 8 ) 
					break;
				
				++run_step ;
				last_val = data[i] ;
				++i ;
			}while( i < size && run_step < 16 ); 	
			
#if defined(DEBUG_COMPRESS) && !defined(NO_DEBUG_OUTPUT)
			fprintf( stderr, "comp_size = %d, run_step = %d at line %d\n", comp_size, run_step, __LINE__ );
#endif
			if( comp_size + 1 + run_step > size )
				return 0; 

			buffer[comp_size] = RLE_NOZERO_LONG2_SIG | ((run_step-1)&RLE_NOZERO_LONG_LENGTH) ;											   
#if defined(DEBUG_COMPRESS) && !defined(NO_DEBUG_OUTPUT)
			fprintf(stderr, "in %d out %d: 0x%2.2X  - %d 8bits things\n", i, comp_size, buffer[comp_size], run_step );
#endif				
			++comp_size;
			do
			{
				buffer[comp_size] = (sign[k]<<7)|(diff[k]-1) ;
				++k ;
#if defined(DEBUG_COMPRESS) && !defined(NO_DEBUG_OUTPUT)
				fprintf(stderr, "0x%2.2X ", buffer[comp_size] );
#endif
				++comp_size ;
			}while( k  < run_step );
		}else		 
		{	
			int k = 0;		/* 9 bit strings */
			do
			{
				if( data[i] == last_val	) 
					break;
				sign[run_step] = (data[i] > last_val )?0:1;
				diff[run_step] = sign[run_step]?last_val-data[i]:data[i]-last_val ;
				if( diff[run_step] <= 128 ) 
					break;
				
				++run_step ;
				last_val = data[i] ;
				++i ;
			}while( i < size && run_step < 16 ); 	

#if defined(DEBUG_COMPRESS) && !defined(NO_DEBUG_OUTPUT)
				fprintf( stderr, "comp_size = %d, run_step = %d at line %d\n", comp_size, run_step, __LINE__ );
#endif
			if( comp_size + 1 + run_step > size )
				return 0; 
			if( sign[0] == 0 ) 
				buffer[comp_size] = RLE_9BIT_SIG | ((run_step-1)&RLE_NOZERO_LONG_LENGTH) ;											   
			else
				buffer[comp_size] = RLE_9BIT_NEG_SIG | ((run_step-1)&RLE_NOZERO_LONG_LENGTH) ;											   
#if defined(DEBUG_COMPRESS) && !defined(NO_DEBUG_OUTPUT)
			fprintf(stderr, "in %d out %d: 0x%2.2X  - %d 9bit things\n", i, comp_size, buffer[comp_size], run_step );
#endif				
			++comp_size;
			do
			{
				buffer[comp_size] = diff[k]-1 ;
				++k ;
#if defined(DEBUG_COMPRESS) && !defined(NO_DEBUG_OUTPUT)
				fprintf(stderr, "0x%2.2X ", buffer[comp_size] );
#endif
				++comp_size ;
			}while( k  < run_step );
		} 
#if defined(DEBUG_COMPRESS) && !defined(NO_DEBUG_OUTPUT)
	   	fprintf(stderr, "\n");
#endif
	
	}	 
	
	return comp_size ;
}	 

static int
rlediff_decompress_bitmap( CARD8 *buffer,  CARD8* data, int size, CARD8 bitmap_value )
{
	unsigned int count ;
	int out_bytes = 0 ;
	int in_bytes = 0 ;
	CARD8 curr_val = 0;

	while( in_bytes < size ) 
	{
		count = ((unsigned int)(data[in_bytes++])+1) ;
		while( --count > 0 ) 
			buffer[out_bytes++] = curr_val ;
		curr_val = (curr_val == bitmap_value)? 0 : bitmap_value ;
	}
	
	LOCAL_DEBUG_OUT( "in_bytes = %d, out_bytes = %d, size = %d", in_bytes, out_bytes, size );
	return out_bytes;
}

static int
rlediff_decompress( CARD8 *buffer,  CARD8* data, int size )
{
	int count ;
	int out_bytes = 1 ;
	int in_bytes = 1 ;
	CARD8 last_val;

	buffer[0] = last_val = data[0] ; 

	while( in_bytes < size ) 
	{
		CARD8 c = data[in_bytes++] ;
#if defined(DEBUG_COMPRESS) && !defined(NO_DEBUG_OUTPUT)
		fprintf(stderr, "in %d out %d: 0x%2.2X \n", in_bytes, out_bytes, c);
#endif				

		if( (c & RLE_ZERO_MASK) == 0 ) 			   
		{
			count = (int)c  + 1 ;
			while( --count >= 0 )  	  
				buffer[out_bytes++] = last_val ;
		}else if( (c & RLE_NOZERO_SHORT_MASK ) == RLE_NOZERO_SHORT_SIG ) 
		{
			count = c & RLE_NOZERO_SHORT_LENGTH ;
			++count ;
			while( --count >= 0 ) 
			{
				CARD8 mod = ((data[in_bytes]>>4)&0x07)+1;
				last_val = (data[in_bytes]&0x80)?last_val - mod : last_val + mod ;
				buffer[out_bytes++] = last_val ;
				if( --count >= 0 )
				{
					mod = (data[in_bytes]&0x07)+1;
					last_val = (data[in_bytes]&0x08)?last_val - mod : last_val + mod ;
					buffer[out_bytes++] = last_val ;
				}
				++in_bytes ;
			}
		}else
		{
			count = c & RLE_NOZERO_LONG_LENGTH ;
			++count ;
			if( (c & RLE_NOZERO_LONG_MASK ) == RLE_NOZERO_LONG1_SIG ) 
			{
				while( --count >= 0 ) 
				{
					CARD8 mod = ((data[in_bytes]>>6)&0x01)+1;
					last_val = (data[in_bytes]&0x80)?last_val - mod : last_val + mod ;
					buffer[out_bytes++] = last_val ;
					if( --count >= 0 )
					{
						mod = ((data[in_bytes]>>4)&0x01)+1;
						last_val = (data[in_bytes]&0x20)?last_val - mod : last_val + mod ;
						buffer[out_bytes++] = last_val ;
						if( --count >= 0 )
						{
							mod = ((data[in_bytes]>>2)&0x01)+1;
							last_val = (data[in_bytes]&0x08)?last_val - mod : last_val + mod ;
							buffer[out_bytes++] = last_val ;
							if( --count >= 0 )
							{
								mod = (data[in_bytes]&0x01)+1;
								last_val = (data[in_bytes]&0x02)?last_val - mod : last_val + mod ;
								buffer[out_bytes++] = last_val ;
							}
						}

					}
					++in_bytes ;
				}
			}else if( (c & RLE_NOZERO_LONG_MASK ) == RLE_NOZERO_LONG2_SIG ) 
			{
				while( --count >= 0 ) 
				{
					CARD8 mod = (data[in_bytes]&0x7F)+1;
					last_val = (data[in_bytes]&0x80)?last_val - mod : last_val + mod ;
					buffer[out_bytes++] = last_val ;
					++in_bytes ;
				}
			}else
			{
				Bool sign = ((c & RLE_NOZERO_LONG_MASK ) == RLE_9BIT_NEG_SIG);
				while( --count >= 0 ) 
				{
					CARD8 mod = data[in_bytes]+1;
					last_val = sign? last_val - mod : last_val + mod ;
					sign = !sign ;
					buffer[out_bytes++] = last_val ;
					++in_bytes ;
				}
			}
		}	 
	}	 
	LOCAL_DEBUG_OUT( "in_bytes = %d, out_bytes = %d, size = %d", in_bytes, out_bytes, size );
	return out_bytes;
}	 


static CARD8* 
compress_stored_data( ASStorage *storage, CARD8 *data, int size, ASFlagType *flags, int *compressed_size, 
					  CARD8 bitmap_threshold )
{
	/* TODO: just a stub for now - need to implement compression */
	int comp_size = size ;
	CARD8  *buffer = data ;
	size_t 	buf_size = size ; 
	
	if( get_flags( *flags, ASStoprage_RLEDiffCompress ) && size > 8 )
	{
		clear_flags( *flags, ASStoprage_RLEDiffCompress );
		if( storage->comp_buf_size < size ) 
		{	
			storage->comp_buf_size = ((size/AS_STORAGE_PAGE_SIZE)+1)*AS_STORAGE_PAGE_SIZE ;
			storage->comp_buf = realloc( storage->comp_buf, storage->comp_buf_size );
		}
		buffer = storage->comp_buf ;
		buf_size = storage->comp_buf_size ;
		if( buffer ) 
		{
			if( get_flags( *flags, ASStorage_Bitmap ) )
				comp_size = rlediff_compress_bitmap( buffer, data, size, bitmap_threshold );
			else 
				comp_size = rlediff_compress( buffer, data, size );

			if( comp_size == 0 )	 
			{	
				buffer = data ;
				comp_size = size ;
			}else
			{	
				set_flags( *flags, ASStoprage_RLEDiffCompress );
				UncompressedSize += size ;
				CompressedSize += comp_size ;
			}
		}else
			buffer = data ;	 
		
		LOCAL_DEBUG_OUT( "size = %d, compressed_size = %d, flags = 0x%lX", size, comp_size, *flags );
	}	 
		
	if( compressed_size ) 
		*compressed_size = comp_size ;
	return buffer;
}

static CARD8 *
decompress_stored_data( ASStorage *storage, CARD8 *data, int size, int uncompressed_size, 
						ASFlagType flags, CARD8 bitmap_value )
{
	CARD8  *buffer = data ;

	LOCAL_DEBUG_OUT( "size = %d, uncompressed_size = %d, flags = 0x%lX", size, uncompressed_size, flags );
	if( get_flags( flags, ASStoprage_RLEDiffCompress ) && uncompressed_size > 8)
	{
		buffer = storage->comp_buf ;
		if( get_flags( flags, ASStorage_Bitmap ) )
			rlediff_decompress_bitmap( buffer, data, size, bitmap_value );	 
		else			
			rlediff_decompress( buffer, data, size );	 
		/* need to check decompressed size */
	}
	
	return buffer;
}

static void
add_storage_slots( ASStorageBlock *block )
{
	int i = block->slots_count ;
	block->slots_count += AS_STORAGE_SLOTS_BATCH ; 
#ifndef DEBUG_ALLOCS
	block->slots = realloc( block->slots, block->slots_count*sizeof(ASStorageSlot*));
#else
	block->slots = guarded_realloc( block->slots, block->slots_count*sizeof(ASStorageSlot*));
	LOCAL_DEBUG_OUT( "reallocated %d slots pointers", block->slots_count );
#endif
	UsedMemory += AS_STORAGE_SLOTS_BATCH*sizeof(ASStorageSlot*) ;
	memset( &(block->slots[i]),	0x00, AS_STORAGE_SLOTS_BATCH*sizeof(ASStorageSlot*) );
}



static ASStorageBlock *
create_asstorage_block( int useable_size )
{
	int allocate_size = (((sizeof(ASStorageBlock)+sizeof(ASStorageSlot) + useable_size)/AS_STORAGE_PAGE_SIZE)+1)*AS_STORAGE_PAGE_SIZE ;
#ifndef DEBUG_ALLOCS
	void *ptr = malloc(allocate_size);
#else
	void *ptr = guarded_malloc(allocate_size);
	LOCAL_DEBUG_OUT( "allocated %d files", allocate_size );
#endif
	UsedMemory += allocate_size ;
	if( ptr == NULL ) 
		return NULL;
	ASStorageBlock *block = ptr ;
	memset( block, 0x00, sizeof(ASStorageBlock));
	block->size = allocate_size - sizeof(ASStorageBlock) ;
	block->total_free = block->size - ASStorageSlot_SIZE ;

	block->slots_count = 0 ;
	add_storage_slots( block ) ;   
	
	if( block->slots == NULL ) 
	{	
		free( ptr ); 
		UsedMemory -= allocate_size ;
		return NULL;
	}
	block->start = (ASStorageSlot*)(ptr+sizeof(ASStorageBlock));
	block->end = (ASStorageSlot*)(ptr+(allocate_size-ASStorageSlot_SIZE));
	block->slots[0] = block->start ;
	block->slots[0]->flags = 0 ;  /* slot of the free memory */ 
	block->slots[0]->ref_count = 0 ;
	block->slots[0]->size = block->size ;
	block->slots[0]->uncompressed_size = block->size ;
	block->last_used = 0;
	block->first_free = 0 ;
	
	LOCAL_DEBUG_OUT("Storage block created : block ptr = %p, slots ptr = %p", block, block->slots );
	
	return block;
}

static void
destroy_asstorage_block( ASStorageBlock *block )
{
	LOCAL_DEBUG_OUT( "freeing block %p, size = %d", block, block->size );
	
	UsedMemory -= block->slots_count * sizeof(ASStorageSlot*) ;
	UsedMemory -= block->size + sizeof(ASStorageBlock) ;

#ifndef DEBUG_ALLOCS
	free( block->slots );
	free( block );	  
#else	
	guarded_free( block->slots );
	guarded_free( block );
#endif

}

static int
select_storage_block( ASStorage *storage, int compressed_size, ASFlagType flags )
{
	int i ;
	int new_block = -1 ; 
	compressed_size += sizeof(ASStorageSlot);
	for( i = 0 ; i < storage->blocks_count ; ++i ) 
	{
		ASStorageBlock *block = storage->blocks[i];
		if( block )
		{	
			if( block->total_free > compressed_size && 
				block->last_used < AS_STORAGE_MAX_SLOTS_CNT )
				return i+1;
		}else if( new_block < 0 ) 
			new_block = i ;
	}		
	/* no available blocks found - need to allocate a new block */
	if( new_block  < 0 ) 
	{
		i = new_block = storage->blocks_count ;
		storage->blocks_count += 16 ;
#ifndef DEBUG_ALLOCS
		storage->blocks = realloc( storage->blocks, storage->blocks_count*sizeof(ASStorageBlock*));
#else
		storage->blocks = guarded_realloc( storage->blocks, storage->blocks_count*sizeof(ASStorageBlock*));
		LOCAL_DEBUG_OUT( "reallocated %d blocks pointers", storage->blocks_count );
#endif		   
		UsedMemory += 16*sizeof(ASStorageBlock*) ;

		while( ++i < storage->blocks_count )
			storage->blocks[i] = NULL ;
	}	 
	storage->blocks[new_block] = create_asstorage_block( max(storage->default_block_size, compressed_size) );		
	if( storage->blocks[new_block] == NULL )  /* memory allocation failed ! */ 
		new_block = -1 ;
	return new_block+1;
}

static inline void
destroy_storage_slot( ASStorageBlock *block, int index )
{
	ASStorageSlot **slots = block->slots ;
	int i = index;
	
	slots[i] = NULL ; 
	if( block->last_used == index ) 
	{	
		while( --i > 0 ) 
			if( slots[i] != NULL ) 
				break;
		block->last_used = i<0?0:i;	 
	}
}

static inline void 
join_storage_slots( ASStorageBlock *block, ASStorageSlot *from_slot, ASStorageSlot *to_slot )
{
	ASStorageSlot *s ;
	do
	{
		s = AS_STORAGE_GetNextSlot(from_slot);
		from_slot->size += ASStorageSlot_FULL_SIZE(s) ;	
		destroy_storage_slot( block, s->index );
	}while( s < to_slot );	
}


static inline void
defragment_storage_block( ASStorageBlock *block )
{
	ASStorageSlot *brk, *next_used, **slots = block->slots ;
	int i, first_free = -1;
	unsigned long total_free = 0 ;
	brk = next_used = block->start ; 
	
	
	for( i = 0 ; i <= block->last_used ; ++i ) 
	{
		if( slots[i] ) 
			if( slots[i]->flags == 0 ) 
				slots[i] = NULL ;
		if( slots[i] == NULL ) 
		{
			if( first_free < 0 ) 
				first_free = i ;
		}
	}
	while( --i > 0 ) 
		if( slots[i] != NULL ) 
			break;
	block->last_used = i ;
								
	while( brk < block->end ) 
	{	
		ASStorageSlot *used = next_used;
		while( used < block->end && used->flags == 0 ) 
			used = AS_STORAGE_GetNextSlot(used);
		LOCAL_DEBUG_OUT("brk = %p, used = %p, end = %p", brk, used, block->end );
		if( used >= block->end ) 
		{
			total_free = (unsigned long)((CARD8*)block->end - (CARD8*)brk);
			if( total_free < ASStorageSlot_SIZE ) 
				total_free = 0 ;
			else
				total_free -= ASStorageSlot_SIZE ; 	
			break;
		}else
			next_used = AS_STORAGE_GetNextSlot(used);

		LOCAL_DEBUG_OUT("used = %p, used->size = %ld", 
						used,used->size );
		if( next_used < block->end ) 
		{
			LOCAL_DEBUG_OUT("next_used = %p, next_used->size = %ld", 
							next_used, next_used->size );
		}

		if( used != brk	)
		{/* can't use memcpy as regions may overlap */
			int size = (ASStorageSlot_FULL_SIZE(used))/4;
			register CARD32 *from = (CARD32*)used ;
			register CARD32 *to = (CARD32*)brk ;
			for( i = 0 ; i < size ; ++i ) 
				to[i] = from[i];
			/* updating pointer : */	
			slots[brk->index] = brk ;
			LOCAL_DEBUG_OUT("brk = %p, brk->size = %ld, index = %d", brk, brk->size, brk->index );
		}	
		brk = AS_STORAGE_GetNextSlot(brk);
	}
	
	if( total_free > 0 )
	{
		if( first_free < 0  ) 
		{
			if( ++block->last_used >= block->slots_count ) 
				add_storage_slots( block );	
			first_free = block->last_used ;
		}
		brk->flags = 0 ;
		brk->size = total_free ; 
		brk->uncompressed_size = total_free ;
		brk->ref_count = 0 ;
		brk->index = first_free ; 
		block->first_free = first_free ;
		
		LOCAL_DEBUG_OUT("brk = %p, brk->size = %ld, index = %d, first_free = %d", 
						 brk, brk->size, brk->index, first_free );
		
		block->slots[first_free] = brk ;
		if( block->last_used < first_free ) 
			block->last_used = first_free ;
	}
	
	block->total_free = total_free ;
	LOCAL_DEBUG_OUT( "total_free after defrag = %ld", total_free );
	
	slots = block->slots ;
	for( i = 0 ; i <= block->last_used ; ++i ) 
		if( slots[i] )
			if( slots[i]->index != i ) 
			{
				LOCAL_DEBUG_OUT( "Storage Integrity check failed - block = %p, index = %d", block, i ) ;	
				exit(0);
			}
	
}

static ASStorageSlot *
select_storage_slot( ASStorageBlock *block, int size )
{
	int i = block->first_free, last_used = block->last_used ;
	ASStorageSlot **slots = block->slots ;
	
	while( i <= last_used )
	{
		ASStorageSlot *slot = slots[i] ;
		LOCAL_DEBUG_OUT( "last_used = %d, slots[%d] = %p", last_used, i, slot );
		if( slot != 0 )
		{
			int size_to_match = size ;
			while( slot->flags == 0 )
			{
				if( ASStorageSlot_USABLE_SIZE(slot) >= size )
					return slot;
				if( ASStorageSlot_USABLE_SIZE(slot) >= size_to_match )
				{
					join_storage_slots( block, slots[i], slot );
					return slots[i];
				}	
				size_to_match -= ASStorageSlot_FULL_SIZE(slot);
				slot = AS_STORAGE_GetNextSlot(slot);		
				/* make sure we has not exceeded boundaries of the block */									   
				if( slot >=  block->end )
					break;
			}			
		}
		++i ;
	}
		
	/* no free slots of sufficient size - need to do defragmentation */
	defragment_storage_block( block );
	i = block->first_free;
    if( block->slots[i] == NULL || block->slots[i]->size < size ) 
		return NULL;
	return block->slots[i];		   
}

static inline Bool
split_storage_slot( ASStorageBlock *block, ASStorageSlot *slot, int to_size )
{
	int old_size = ASStorageSlot_USABLE_SIZE(slot) ;
	ASStorageSlot *new_slot ;

	slot->size = to_size ; 
	new_slot = AS_STORAGE_GetNextSlot(slot);
	if( new_slot >=  block->end )
		return True;

	new_slot->flags = 0 ;
	new_slot->ref_count = 0 ;
	new_slot->size = old_size - ASStorageSlot_USABLE_SIZE(slot) - ASStorageSlot_SIZE ;											   
	new_slot->uncompressed_size = 0 ;
	
	new_slot->index = 0 ;
	/* now we need to find where this slot's pointer we should store */		   
	if( block->unused_count < block->slots_count/10 && block->last_used < block->slots_count-1 )
	{	
		new_slot->index = ++(block->last_used) ;
	}else
	{
		register int i, max_i = block->slots_count ;
		register ASStorageSlot **slots = block->slots ;
		for( i = 0 ; i < max_i ; ++i ) 
			if( slots[i] == NULL ) 
				break;
		if( i >= max_i ) 
		{
			if( block->slots_count + AS_STORAGE_SLOTS_BATCH > AS_STORAGE_MAX_SLOTS_CNT )
				return False;
			else
			{
				i = block->slots_count ;
				block->last_used = i ;
				add_storage_slots( block );
				slots = block->slots ;
			}	 
		}
 		new_slot->index = i ;		
		if( i < block->last_used )
			--(block->unused_count);
	}	
	block->slots[new_slot->index] = new_slot ;
	return True;
}

static int
store_data_in_block( ASStorageBlock *block, CARD8 *data, int size, int compressed_size, ASFlagType flags )
{
	ASStorageSlot *slot ;
	CARD8 *dst ;
	slot = select_storage_slot( block, compressed_size );
	LOCAL_DEBUG_OUT( "selected slot %p for size %d (compressed %d) and flags %lX", slot, size, compressed_size, flags );
	if( slot == NULL ) 
		return 0;
	if( ASStorageSlot_USABLE_SIZE(slot) >= compressed_size+ASStorageSlot_SIZE ) 
		if( !split_storage_slot( block, slot, compressed_size ) ) 
			return 0;

	block->total_free -= ASStorageSlot_FULL_SIZE(slot);
	
	dst = &(slot->data[0]);
	memcpy( dst, data, compressed_size );
	slot->flags = (flags | ASStorage_Used) ;
	slot->ref_count = 0;
	slot->size = compressed_size ;
	slot->uncompressed_size = size ;

	if( slot->index == block->first_free ) 
	{
		int i = block->first_free ;
		while( ++i < block->last_used ) 
			if( block->slots[i] && block->slots[i]->flags == 0 ) 
				break;
		block->first_free = i ;
	}
		  
	return slot->index+1 ;
}


static inline ASStorageBlock *
find_storage_block( ASStorage *storage, ASStorageID id )
{	
	int block_idx = StorageID2BlockIdx(id);
	if( block_idx >= 0 && block_idx < storage->blocks_count )  
		return storage->blocks[block_idx];
	return NULL ;
}

static ASStorageSlot *
find_storage_slot( ASStorageBlock *block, ASStorageID id )
{	
	if( block != NULL ) 
	{
		int slot_idx = StorageID2SlotIdx(id);
		if( slot_idx >= 0 && slot_idx < block->slots_count ) 
		{
			if( block->slots[slot_idx] && block->slots[slot_idx]->flags != 0 )
				return block->slots[slot_idx];
		}
	}	
	return NULL ;
}

static inline void 
free_storage_slot( ASStorageBlock *block, ASStorageSlot *slot)
{
	slot->flags = 0 ;
	block->total_free += ASStorageSlot_USABLE_SIZE(slot) ;
}	 

static Bool 
is_block_empty( ASStorageBlock *block)
{
	int i = block->last_used+1;
	ASStorageSlot **slots = block->slots ;
	while( --i >= 0 ) 
	{
		if( slots[i] )
			if( slots[i]->flags != 0 ) 
				return False;	 
	}	
	return True;	
}	 

static void 
free_storage_block( ASStorage *storage, int block_idx  )
{
	ASStorageBlock *block = storage->blocks[block_idx] ;
	storage->blocks[block_idx] = NULL ;
	destroy_asstorage_block( block );
}	 

static ASStorageSlot *
convert_slot_to_ref( ASStorage *storage, ASStorageID id )	
{
	int block_idx = StorageID2BlockIdx(id);
	ASStorageBlock *block = find_storage_block(storage, id);
	ASStorageID target_id = 0;
	int slot_id = 0 ;
	int ref_index, body_index ;
	ASStorageSlot *ref_slot, *body_slot ;
	
	if( block->total_free < sizeof(ASStorageID))
		return NULL ; 
	
	slot_id = store_data_in_block(  block, (CARD8*)&target_id, 
									sizeof(ASStorageID), sizeof(ASStorageID), 
									ASStorage_Reference );
	if( slot_id <= 0 )
		return NULL ; 
	
	/* now we need to swap contents of the slots */
	ref_index = slot_id-1 ;
	ref_slot = block->slots[ref_index] ;
	
	body_index = StorageID2SlotIdx(id) ; 
	body_slot = block->slots[body_index] ;
	
	block->slots[ref_index] = body_slot ;
	body_slot->index = ref_index ;

	block->slots[body_index] = ref_slot ; 
	ref_slot->index = body_index ;

	target_id = make_asstorage_id( block_idx+1, slot_id );
	memcpy( &(ref_slot->data[0]), (CARD8*)&target_id, sizeof(ASStorageID));

	body_slot->ref_count = 1;

	return ref_slot;
}

/************************************************************************/
/* Public Functions : 													*/
/************************************************************************/
ASStorage *
create_asstorage()
{
#ifndef DEBUG_ALLOCS
	ASStorage *storage = calloc(1, sizeof(ASStorage));
#else
	ASStorage *storage = guarded_calloc(1, sizeof(ASStorage));
#endif
	UsedMemory += sizeof(ASStorage) ;
	if( storage )
		storage->default_block_size = AS_STORAGE_DEF_BLOCK_SIZE ;
	return storage ;
}

void 
destroy_asstorage(ASStorage **pstorage)
{
	ASStorage *storage = *pstorage ;
	
	if( storage ) 
	{	
		if( storage->blocks != NULL || storage->blocks_count  > 0 )
		{
			int i ;
			for( i = 0 ; i < storage->blocks_count ; ++i ) 
				if( storage->blocks[i] ) 
					destroy_asstorage_block( storage->blocks[i] );
			UsedMemory -= storage->blocks_count * sizeof(ASStorageBlock*) ;
#ifndef DEBUG_ALLOCS
			free( storage->blocks );
#else	
			guarded_free( storage->blocks );
#endif

		}	
		UsedMemory -= sizeof(ASStorage) ;
#ifndef DEBUG_ALLOCS
		free( storage );
#else	
		guarded_free( storage );
#endif
		*pstorage = NULL;
	}
}

ASStorage *
get_default_asstorage()
{
	if( _as_default_storage == NULL )
		_as_default_storage = create_asstorage();
	return _as_default_storage ;
}

void 
flush_default_asstorage()
{
	if( _as_default_storage != NULL )
		destroy_asstorage(&_as_default_storage);
}

ASStorageID 
store_data(ASStorage *storage, CARD8 *data, int size, ASFlagType flags, CARD8 bitmap_threshold)
{
	int id = 0 ;
	int block_id ;
	int slot_id ;
	int compressed_size = size ;
	CARD8 *buffer = data;

	if( storage == NULL ) 
		storage = get_default_asstorage();

	LOCAL_DEBUG_CALLER_OUT( "data = %p, size = %d, flags = %lX", data, size, flags );
	if( size <= 0 || data == NULL || storage == NULL ) 
		return 0;
	if( bitmap_threshold == 0 ) 
		bitmap_threshold = AS_STORAGE_DEFAULT_BMAP_THRESHOLD ;
	if( get_flags( flags, ASStorage_CompressionType ) && !get_flags(flags, ASStorage_Reference))
		buffer = compress_stored_data( storage, data, size, &flags, &compressed_size, bitmap_threshold );
	block_id = select_storage_block( storage, compressed_size, flags );
	LOCAL_DEBUG_OUT( "selected block %d", block_id );
	if( block_id > 0 ) 
	{
		slot_id = store_data_in_block(  storage->blocks[block_id-1], 
										buffer, size, compressed_size, flags );

		if( slot_id > 0 )	
			id = make_asstorage_id( block_id, slot_id );
	}
	return id ;		
}



int  
fetch_data(ASStorage *storage, ASStorageID id, CARD8 *buffer, int offset, int buf_size, CARD8 bitmap_value)
{
	if( storage == NULL ) 
		storage = get_default_asstorage();
	
	if( storage != NULL && id != 0 )
	{	
		ASStorageSlot *slot = find_storage_slot( find_storage_block( storage, id ), id );
		LOCAL_DEBUG_OUT( "slot = %p", slot );
		if( slot )
		{
			int uncomp_size = slot->uncompressed_size ;
			if( get_flags( slot->flags, ASStorage_Reference) )
			{
				ASStorageID target_id = 0;
			 	memcpy( &target_id, &(slot->data[0]), sizeof( ASStorageID ));				   
				LOCAL_DEBUG_OUT( "target_id = %ld", target_id );
				return fetch_data(storage, target_id, buffer, offset, buf_size, bitmap_value);
			}	 

			LOCAL_DEBUG_OUT( "flags = %X, index = %d, size = %ld, uncompressed_size = %d", 
							 slot->flags, slot->index, slot->size, uncomp_size );
			if( bitmap_value == 0 ) 
				bitmap_value = AS_STORAGE_DEFAULT_BMAP_VALUE ;

			if( buffer && buf_size > 0 ) 
			{
				int bytes_in = 0;
				CARD8 *tmp = decompress_stored_data( storage, &(slot->data[0]), slot->size,
														uncomp_size, slot->flags, bitmap_value );
				while( offset > uncomp_size ) offset -= uncomp_size ; 
				while( offset < 0 ) offset += uncomp_size ; 
				if( offset > 0 ) 
				{
					bytes_in = uncomp_size-offset ; 
					if( bytes_in  < buf_size ) 
						bytes_in = buf_size ;
					memcpy( buffer, tmp+offset, bytes_in ); 															
				}
				while( bytes_in  < buf_size ) 
				{
					int to_copy = buf_size - bytes_in ; 
					if( to_copy > uncomp_size ) 
						to_copy = uncomp_size ;
					memcpy( buffer+bytes_in, tmp, to_copy ); 															
					bytes_in += to_copy;
				}
			}
			return uncomp_size ;
		}
	}
	return 0 ;	
}

void 
forget_data(ASStorage *storage, ASStorageID id)
{
	if( storage == NULL ) 
		storage = get_default_asstorage();
	
	if( storage != NULL && id != 0 ) 
	{
		ASStorageBlock *block = find_storage_block( storage, id );
 		ASStorageSlot  *slot  = find_storage_slot( block, id );				
		if( block && slot ) 
		{
			if( get_flags( slot->flags, ASStorage_Reference) )
			{
				ASStorageID target_id = 0;
			 	memcpy( &target_id, &(slot->data[0]), sizeof( ASStorageID ));				   
				forget_data( storage, target_id );					
			}	 
			if( slot->ref_count > 1 ) 
				--(slot->ref_count);
			else
			{	
				free_storage_slot(block, slot);
				if( is_block_empty(block) ) 
					free_storage_block( storage, StorageID2BlockIdx(id) );
			}
		}	 
	}			  
}

ASStorageID 
dup_data(ASStorage *storage, ASStorageID id)
{
	ASStorageID new_id = 0 ;

	if( storage == NULL ) 
		storage = get_default_asstorage();
	   
	if( storage != NULL && id != 0 )
	{	
		ASStorageSlot *slot = find_storage_slot( find_storage_block( storage, id ), id );
		LOCAL_DEBUG_OUT( "slot = %p", slot );
		if( slot )
		{
			ASStorageSlot *target_slot;
			ASStorageID target_id = id ;
			if( !get_flags( slot->flags, ASStorage_Reference )) 
			{	
				if( (slot = convert_slot_to_ref( storage, id )) == NULL ) 
					return 0;
			}
			
			memcpy( &target_id, &(slot->data[0]), sizeof( ASStorageID ));
			/* from now on - slot is a reference slot, so we just need to 
			 * duplicate it and increase ref_count of target */
			target_slot = find_storage_slot( find_storage_block( storage, target_id ), target_id );
			if( target_slot == NULL ) 
				return 0;
			/* doing it here as store_data() may change slot pointers */
			++(target_slot->ref_count);			   

			new_id = store_data( storage, (CARD8*)&target_id, sizeof(ASStorageID), ASStorage_Reference, 0);
		}
	}
	return new_id;
}

/*************************************************************************/
/* test code */
/*************************************************************************/
#ifdef TEST_ASSTORAGE
#include "afterimage.h"

#define STORAGE_TEST_KINDS	5
static int StorageTestKinds[STORAGE_TEST_KINDS][2] = 
{
	{1024, 4096 },
	{128*1024, 64 },
	{256*1024, 32 },
	{512*1024, 16 },
	{1024*1024, 8 }
};	 

CARD8 Buffer[1024*1024] ;
/*#define STORAGE_TEST_COUNT  800*/
#define STORAGE_TEST_COUNT  8+16+32+64+4096
typedef struct ASStorageTest {
	int size ;
	CARD8 *data;
	Bool linked ;
	ASStorageID id ;
}ASStorageTest;
 
static ASStorageTest Tests[STORAGE_TEST_COUNT];

static ASImageDecoder *imdec = NULL ;

void
make_storage_test_data( ASStorageTest *test, int min_size, int max_size )
{
	int size = random()%max_size ;
	int i ;
	static CARD32 rnd32_seed = 345824357;
	static int chan = 0 ;
	CARD32 *data ;

#define MAX_MY_RND32		0x00ffffffff
#ifdef WORD64
#define MY_RND32() \
(rnd32_seed = ((1664525L*rnd32_seed)&MAX_MY_RND32)+1013904223L)
#else
#define MY_RND32() \
(rnd32_seed = (1664525L*rnd32_seed)+1013904223L)
#endif
	
	if( size <= min_size )
		size += min_size ;
 	
	test->size = size ; 	   
#ifndef DEBUG_ALLOCS
	test->data = malloc(size);
#else
	test->data = guarded_malloc(size);
#endif

	test->linked = False ;
	

	if( imdec ) 
	{	
		int k = 0;
		if( chan == 0 ) 
			imdec->decode_image_scanline( imdec );
		data = imdec->buffer.channels[chan];
		++chan ; 
		if( chan >= 3 ) 
			chan = 0 ;
		for( i = 0 ; i < size ; ++i ) 
		{	
			test->data[i] = data[k] ;
			++k ;
			if( k >= imdec->im->width )
				k = 0 ;
		}
	}else
	{	
		for( i = 0 ; i < size ; ++i ) 
			test->data[i] = MY_RND32() ;
	}
	test->id = 0 ;
}

int 
test_data_integrity( CARD8 *a, CARD8* b, int size, ASFlagType flags ) 
{
	register int i ;
	for( i = 0 ; i < size ; ++i ) 
	{
		Bool fail = False ;
		if( get_flags( flags, ASStorage_Bitmap ) )
			fail = ( (a[i] >  AS_STORAGE_DEFAULT_BMAP_THRESHOLD && b[i] <= AS_STORAGE_DEFAULT_BMAP_THRESHOLD )||
				     (a[i] <= AS_STORAGE_DEFAULT_BMAP_THRESHOLD && b[i] >  AS_STORAGE_DEFAULT_BMAP_THRESHOLD));
		else
			fail = ( a[i] != b[i] );
				
		if( fail ) 
		{
			int k ;
			fprintf( stderr, "\tBytes %d differ : a[%d] == 0x%2.2X, b[%d] == 0x%2.2X\na: ", i, i, a[i], i, b[i] );
			for( k = 0 ; k < size ; ++k ) 
				fprintf( stderr, (k==i)?"##%2.2X## ":"%2.2X ", a[k] );
			fprintf( stderr, "\nb: " );
			for( k = 0 ; k < size ; ++k ) 
				fprintf( stderr, (k==i)?"##%2.2X## ":"%2.2X ", b[k] );
			fprintf( stderr, "\n" );
			return a[i]-b[i];			
		}			   
	}
	return 0 ;
}
	  
Bool 
test_asstorage(Bool interactive, ASFlagType test_flags )
{
	ASStorage *storage ;
	ASStorageID id ;
	int i, kind, test_count;
	int min_size, max_size ;

	UsedMemory = 0 ;
	UncompressedSize = 0 ;
	CompressedSize = 0 ;
	   
	fprintf(stderr, "Testing storage creation ...");
	storage = create_asstorage();
#define TEST_EVAL(val)   do{ \
							if(!(val)){ fprintf(stderr, "failed\n"); return 1;} \
							else fprintf(stderr, "success.\n");}while(0)
	TEST_EVAL( storage != NULL ); 
	
	fprintf(stderr, "Testing store_data for data %p size = %d, and flags 0x%lX...", NULL, 0,
			test_flags);
	id = store_data( storage, NULL, 0, test_flags, 0 );
	TEST_EVAL( id == 0 ); 

	kind = 0 ; 
	min_size = 1 ;
	max_size = StorageTestKinds[kind][0] ; 
	test_count = StorageTestKinds[kind][1] ;
	for( i = 0 ; i < STORAGE_TEST_COUNT ; ++i ) 
	{
		make_storage_test_data( &(Tests[i]), min_size, max_size );
		fprintf(stderr, "Testing store_data for data %p size = %d, and flags 0x%lX...", Tests[i].data, Tests[i].size,
				test_flags);
		Tests[i].id = store_data( storage, Tests[i].data, Tests[i].size, test_flags, 0 );
		TEST_EVAL( Tests[i].id != 0 ); 
		fprintf(stderr, "\tstored with id = %lX...\n", Tests[i].id );

		if( --test_count <= 0 )
		{
			if( ++kind >= STORAGE_TEST_KINDS ) 
				break;
			min_size = max_size ;
			max_size = StorageTestKinds[kind][0] ; 
			test_count = StorageTestKinds[kind][1] ;
		}		   
	}	 

	fprintf( stderr, "%d :memory used %d #####################################################\n", __LINE__, UsedMemory );
	fprintf( stderr, "%d :compressed_size = %d, uncompressed_size = %d, ratio = %d %% ###########\n", __LINE__, CompressedSize, UncompressedSize, (UncompressedSize==0)?0:(CompressedSize/(UncompressedSize/100)) );
	if( interactive )
	   fgetc(stdin);
	for( i = 0 ; i < STORAGE_TEST_COUNT ; ++i ) 
	{
		int size ;
		int res ;
		fprintf(stderr, "Testing fetch_data for id %lX size = %d ...", Tests[i].id, Tests[i].size);
		size = fetch_data(storage, Tests[i].id, &(Buffer[0]), 0, Tests[i].size, 0);
		TEST_EVAL( size == Tests[i].size ); 
		
		fprintf(stderr, "Testing fetched data integrity ...");
		res = test_data_integrity( &(Buffer[0]), Tests[i].data, size, test_flags );
		TEST_EVAL( res == 0 ); 
	}	 

	fprintf( stderr, "%d :memory used %d #####################################################\n", __LINE__, UsedMemory );
	if( interactive )
	   fgetc(stdin);
	for( i = 0 ; i < STORAGE_TEST_COUNT ; ++i ) 
	{
		int size ;
		int r = random();
		if( (r&0x01) == 0 || Tests[i].id == 0 ) 
			continue;
		fprintf(stderr, "%d: Testing forget_data for id %lX size = %d ...\n", __LINE__, Tests[i].id, Tests[i].size);
		forget_data(storage, Tests[i].id);
		size = fetch_data(storage, Tests[i].id, &(Buffer[0]), 0, Tests[i].size, 0 );
		TEST_EVAL( size != Tests[i].size ); 
		Tests[i].id = 0;
#ifndef DEBUG_ALLOCS
		free( Tests[i].data );
#else
		guarded_free( Tests[i].data );
#endif
		Tests[i].data = NULL ; 
		Tests[i].size = 0 ;
	}	 
	fprintf( stderr, "%d :memory used %d #####################################################\n", __LINE__, UsedMemory );
	if( interactive )
	   fgetc(stdin);
	kind = 0 ; 
	min_size = 1 ;
	max_size = StorageTestKinds[kind][0] ; 
	test_count = StorageTestKinds[kind][1] ;
	for( i = 0 ; i < STORAGE_TEST_COUNT ; ++i ) 
	{
		if( Tests[i].id == 0 ) 
		{	
			make_storage_test_data( &(Tests[i]), min_size, max_size );
			fprintf(stderr, "Testing store_data for data %p size = %d, and flags 0x%lX...\n", Tests[i].data, Tests[i].size,
					test_flags);
			Tests[i].id = store_data( storage, Tests[i].data, Tests[i].size, test_flags, 0 );
			TEST_EVAL( Tests[i].id != 0 ); 
		}
		if( --test_count <= 0 )
		{
			if( ++kind >= STORAGE_TEST_KINDS ) 
				break;
			min_size = max_size ;
			max_size = StorageTestKinds[kind][0] ; 
			test_count = StorageTestKinds[kind][1] ;
		}		   
	}	 
	fprintf( stderr, "%d :memory used %d #####################################################\n", __LINE__, UsedMemory );
	if( interactive )
	   fgetc(stdin);
	for( i = 0 ; i < STORAGE_TEST_COUNT ; ++i ) 
	{
		int size ;
		int res ;
		fprintf(stderr, "Testing fetch_data for id %lX size = %d ...", Tests[i].id, Tests[i].size);
		size = fetch_data(storage, Tests[i].id, &(Buffer[0]), 0, Tests[i].size, 0);
		TEST_EVAL( size == Tests[i].size ); 
		
		fprintf(stderr, "Testing fetched data integrity ...");
		res = test_data_integrity( &(Buffer[0]), Tests[i].data, size, test_flags );
		TEST_EVAL( res == 0 ); 
	}	 

	fprintf( stderr, "%d :memory used %d #####################################################\n", __LINE__, UsedMemory );
	if( interactive )
	   fgetc(stdin);
	for( i = 0 ; i < STORAGE_TEST_COUNT ; ++i ) 
	{
		int size ;
		int r = random();
		if( (r&0x01) == 0 || Tests[i].id == 0 ) 
			continue;
		fprintf(stderr, "%d: Testing forget_data for id %lX size = %d ...\n", __LINE__, Tests[i].id, Tests[i].size);
		forget_data(storage, Tests[i].id);
		size = fetch_data(storage, Tests[i].id, &(Buffer[0]), 0, Tests[i].size, 0);
		TEST_EVAL( size != Tests[i].size ); 
		Tests[i].id = 0;
#ifndef DEBUG_ALLOCS
		free( Tests[i].data );
#else
		guarded_free( Tests[i].data );
#endif
		Tests[i].data = NULL ; 
		Tests[i].size = 0 ;
	}	 

	fprintf( stderr, "%d :memory used %d #####################################################\n", __LINE__, UsedMemory );
	if( interactive )
		fgetc(stdin);
	for( i = 0 ; i < STORAGE_TEST_COUNT ; ++i ) 
	{
		int k, size, res ;
		
		if( Tests[i].id != 0 ) 
			continue;
			
		for( k = i+1 ; k < STORAGE_TEST_COUNT ; ++k ) 
			if( Tests[k].id != 0 ) 
				break;
		if( k >= STORAGE_TEST_COUNT ) 
			for( k = i ; k >= 0 ; --k ) 
				if( Tests[k].id != 0 ) 
					break;
			
		fprintf(stderr, "Testing dup_data for id %lX size = %d ...\n", Tests[k].id, Tests[k].size);
		Tests[i].id = dup_data(storage, Tests[k].id );
		TEST_EVAL( Tests[i].id != 0 ); 
		fprintf(stderr, "Testing dupped data fetching ...\n");
		Tests[i].size = Tests[k].size ;
		Tests[i].data = Tests[k].data ;
		Tests[i].linked = True ;
		size = fetch_data(storage, Tests[i].id, &(Buffer[0]), 0, Tests[i].size, 0);
		TEST_EVAL( size == Tests[i].size ); 
		
		fprintf(stderr, "Testing dupped data integrity ...\n");
		res = test_data_integrity( &(Buffer[0]), Tests[i].data, size, test_flags );
		TEST_EVAL( res == 0 ); 
	
	}	 

	fprintf( stderr, "%d :memory used %d #####################################################\n", __LINE__, UsedMemory );
	if( interactive )
	   fgetc(stdin);
	for( i = 0 ; i < STORAGE_TEST_COUNT ; ++i ) 
	{
		int size ;
		int r = random();
		if( (r&0x01) == 0 || Tests[i].id == 0 ) 
			continue;
		fprintf(stderr, "%d: Testing forget_data for id %lX size = %d ...\n", __LINE__, Tests[i].id, Tests[i].size);
		forget_data(storage, Tests[i].id);
		size = fetch_data(storage, Tests[i].id, &(Buffer[0]), 0, Tests[i].size, 0);
		TEST_EVAL( size != Tests[i].size ); 
		Tests[i].id = 0;
		if( !Tests[i].linked ) 
		{	
#ifndef DEBUG_ALLOCS
			free( Tests[i].data );
#else
			guarded_free( Tests[i].data );
#endif
		}else
			Tests[i].linked = False ;
		Tests[i].data = NULL ; 
		Tests[i].size = 0 ;
	}	 
	
	fprintf( stderr, "%d :memory used %d #####################################################\n", __LINE__, UsedMemory );
	if( interactive )
	   fgetc(stdin);
	kind = 0 ; 
	min_size = 1 ;
	max_size = StorageTestKinds[kind][0] ; 
	test_count = StorageTestKinds[kind][1] ;
	for( i = 0 ; i < STORAGE_TEST_COUNT ; ++i ) 
	{
		if( Tests[i].id == 0 ) 
		{	
			make_storage_test_data( &(Tests[i]), min_size, max_size );
			fprintf(stderr, "Testing store_data for data %p size = %d, and flags 0x%lX...\n", Tests[i].data, Tests[i].size,
					test_flags);
			Tests[i].id = store_data( storage, Tests[i].data, Tests[i].size, test_flags, 0 );
			TEST_EVAL( Tests[i].id != 0 ); 
		}
		if( --test_count <= 0 )
		{
			if( ++kind >= STORAGE_TEST_KINDS ) 
				break;
			min_size = max_size ;
			max_size = StorageTestKinds[kind][0] ; 
			test_count = StorageTestKinds[kind][1] ;
		}		   
	}	 
	fprintf( stderr, "%d :memory used %d #####################################################\n", __LINE__, UsedMemory );
	fprintf( stderr, "%d :compressed_size = %d, uncompressed_size = %d, ratio = %d %% ###########\n", __LINE__, CompressedSize, UncompressedSize, (UncompressedSize==0)?0:(CompressedSize/(UncompressedSize/100)) );

	fprintf(stderr, "Testing storage destruction ...");
	destroy_asstorage(&storage);
	TEST_EVAL( storage == NULL ); 

	for( i = 0 ; i < STORAGE_TEST_COUNT ; ++i ) 
		if( Tests[i].data ) 
		{
			if( !Tests[i].linked ) 
			{	
#ifndef DEBUG_ALLOCS
				free( Tests[i].data );
#else
				guarded_free( Tests[i].data );
#endif
			}
			Tests[i].data = NULL ;
			Tests[i].size = 0 ;
		}

	return 0 ;
}

int main(int argc, char **argv )
{
	Bool interactive = False ; 
	ASImage *im = NULL ;
	int i ;
	int res ;
	
	set_output_threshold( 10 );
	
	for( i = 1 ; i < argc ; ++i ) 
	{	
		fprintf( stderr, "i = %d argv = \"%s\"\n", i, argv[i] );
		if( strcmp(argv[i], "-i") == 0 ) 
			interactive = True ; 
		else if( i+1 <= argc && strcmp(argv[i], "-s") == 0 ) 
		{
			fprintf( stderr, "Loading test source image \"%s\"\n", argv[i+1] );
			im = file2ASImage( argv[i+1], 0xFFFFFFFF, SCREEN_GAMMA, 0, NULL );	  
			++i ;
		}
	}

	if( im ) 
	{	
		imdec = start_image_decoding(NULL, im, SCL_DO_ALL, 0, 0, im->width, im->height, NULL);
		fprintf( stderr, "imdec = %p\n", imdec );
	}

	res = test_asstorage(interactive, 0);
	if( res == 0 )
		res = test_asstorage(interactive, ASStoprage_RLEDiffCompress);
	if( res == 0 )
		res = test_asstorage(interactive, ASStoprage_RLEDiffCompress|ASStorage_Bitmap);
	
	stop_image_decoding( &imdec );
	return res;
}
#endif

