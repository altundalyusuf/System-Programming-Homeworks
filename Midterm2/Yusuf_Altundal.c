/* Yusuf Altundal - 19120205057
 * Bu programda kütüphane fonksiyonları yerine elle malloc ve free fonksiyonları yazılmaya çalışılmıştır.
 *  */


#include <stdio.h>
#include "mymalloc.h"
#include <unistd.h>

int firstCall = 0; 														/* Alan ayırmada ilk kez çağrıldığını kontrol edecek */
int firstSplit =0; 														/* Split işleminde ilk kez çağrıldığını kontrol edecek */

Block *split(Block *b, size_t size)  									/* Kendisine gönderilen hafıza bloğunu gönderilen boyut değerine göre 2 parçaya ayırır*/
{
		
	Block *b1 = b; 											/*1. blok başlangıç adresi*/
	b1->info.size = size ; 					 				/* istenen boyutta blok oluşturuldu*/
	b1->info.isfree = 0 ;
	
	Info *btag =  (Info*)b1 + sizeof(Block)+ b1->info.size;  			/* Boundary Tag */
	btag->isfree= 0;
	btag->size= b1->info.size; 									
	
	/* 2. blok için info bilgileri ve boundary tag*/
	
	Block *b2 = b + sizeof(Block) + size + sizeof(Info); 						/* 2. bloğun başlangıç adresi*/
	b2->info.isfree = 1;
	b2->info.size = b->info.size - size - sizeof(Block) - sizeof(Info) ; 		/* 2. bloğun alanı*/
	
	Info *btag2 =  (Info*)b2 + sizeof(Block)+ b2->info.size;  			/* Boundary Tag */
	btag2->isfree= 1;
	btag2->size= b2->info.size; 
	
	if(firstSplit) 					/* Split fonksiyonu ilk kez çağırıldıysa tek olan bloğu free list ten kaldırıp oluşan boş bloğu ekle*/
	{
		free_list= b2;
		firstSplit = 1;
	}
	else 							/* ilk kez çağrılmadıysa fonksiyona yollanan bloğu free listten çıkar ve oluşan boş bloğu ekle */
	{	
		free_list->prev = b->prev;
	    free_list->next = (struct block*)b2;
	}
	
	
	
	return b1; 
} 

void *mymalloc(size_t size) 										/* Hafızadan alan ayırma fonksiyonu*/
{
	if(firstCall) 													/* Eğer fonksiyon ilk defa çağrıldıysa 1024 lük alan ayırır */
	{
		heap_start = sbrk(SIZE);  																		/* Ayırdığımız 1024 lük alanın başını heap start içinde tutuyoruz */
		heap_start->info.isfree = 1 ;
		heap_start->info.size = (int)((SIZE - sizeof(Block) - sizeof(Info))/16)*16; 					/*16'nın katı size değeri ayırır */
		
		free_list = heap_start; 									/* Ayrılan ilk alanı koca bir blok olarak free list e ekliyoruz */
		free_list->next = NULL;
		free_list->prev = NULL;
		
		firstCall = 1;
	}
	if(heap_start == (void*)-1)
		return NULL;
		
		/* First Fit Arama Algoritması*/
		
	Block *sB= heap_start; 												/* Search Block - Aramayı yapmak için kullanacağımız block */
	char*heap_end= (char*)heap_start + SIZE;  							/* Aramanın devam edeceği sınır yani ayırdığımız 1024 lük alanın sonu*/
	
	/* Verilen alana uygun karşımıza çıkan ilk boş bloğu arama */
	
	while(sB->info.isfree !=1 && (char*)sB < heap_end && sB->info.size >= size)  
	{  
		sB = sB + sizeof(Block) + sizeof(Info) + sB->info.size;		
	}
		if(sB->info.isfree==1)						/* Eğer bloğu bulduysak */
		{ 
			if(sB->info.size == size)				/* eğer blok istediğimiz alanla tam uyuşuyorsa bir şey yapma, datayı döndür */
			{ 
				return sB->data;
			}
			
 			if(sB->info.size >= size)				/* Bulunan blok istenenden büyükse bölücez  (split)*/
			{ 
				Block *newblock = split(sB,size);
				return newblock->data; 				/* Böldükten sonra istenen bloğun datası döndürülecek */
			}  
			
		}
		return NULL;
}

void *myfree(void *p)
{ 						/* Verilen alanı serbest bırakıp etrafında boş blok varsa birkeştiren fonksiyon */
	
	Block *b = p - sizeof(Block);         
	b->info.isfree = 1;
	free_list->next = (struct block*)b;
	
	/* Left Coalescing */
	if(!(b<=heap_start)){
		Info *prev_btag = (Info*)b - sizeof(Info);
		if(prev_btag->isfree == 1)
		{
			Block *prevB= (Block*) prev_btag - prev_btag->size - sizeof(Block);
			prevB->info.size = b->info.size + sizeof(Block) + sizeof(Info);
			Info *btag= (Info*)(prevB->info.size +sizeof(Block));
			btag->isfree = 1;
			btag->size = prevB->info.size;
			b = prevB;

		}
	 }
	 
	 /*Right Coalescing*/
	 
	 if(!(b<=heap_start)){
		Block *nextB= (Block*)b + sizeof(Block) + sizeof(Info);
		if(nextB->info.isfree == 1)
		{
			b->info.size = b->info.size + sizeof(Info) + sizeof(Block) + nextB->info.size;
			Info *btag = (Info*)b + sizeof(Block) + b->info.size;
			btag->isfree = 1;
			btag->size = b->info.size;
			
		}
	 }
	 
	return NULL;  
} 

/*	External fragmentation için en iyi yöntem best-fit, sonra first fit, en kötüsü ise worst fit'tir. */

void printHeap() 	 				/* Heap'teki tüm blokların metadatalarını bastıran fonksiyon*/
{
	Block *search= heap_start;  					/* En baştan heap sonunda gidebilmesi için bir blok oluşturduk ve heap start başlangıç değerini atadık */
	char* heap_end = (char*)heap_start + SIZE;  						/* Nerede duracağını bilmesi için end değişkeni oluşturduk */
	while((char*)search < heap_end )
	{		
		printf("Blocks\n Free: %d\n Size: %d\n---------------------------",search->info.isfree,search->info.size);
		search = search + sizeof(Block) + sizeof(Info) + search->info.size;	
	}
}
int main(int argc, char **argv)
{
	printHeap();
	return 0;
}

