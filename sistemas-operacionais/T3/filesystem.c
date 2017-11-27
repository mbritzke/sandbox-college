#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include "libdisksimul.h"
#include "filesystem.h"


int posEntries = 0;
int tableEntries = 0;

/**
 * @brief Format disk.
 * 
 */
int fs_format(){
	int ret, i;
	struct root_table_directory root_dir;
	struct sector_data sector;
	
	if ( (ret = ds_init(FILENAME, SECTOR_SIZE, NUMBER_OF_SECTORS, 1)) != 0 ){
		return ret;
	}
	
	memset(&root_dir, 0, sizeof(root_dir));
	
	root_dir.free_sectors_list = 1; /* first free sector. */
	
	ds_write_sector(0, (void*)&root_dir, SECTOR_SIZE);
	
	/* Create a list of free sectors. */
	memset(&sector, 0, sizeof(sector));
	
	for(i=1;i<NUMBER_OF_SECTORS;i++){
		if(i<NUMBER_OF_SECTORS-1){
			sector.next_sector = i+1;
		}else{
			sector.next_sector = 0;
		}
		ds_write_sector(i, (void*)&sector, SECTOR_SIZE);
	}
	
	ds_stop();
	
	printf("Disk size %d kbytes, %d sectors.\n", (SECTOR_SIZE*NUMBER_OF_SECTORS)/1024, NUMBER_OF_SECTORS);
	
	return 0;
}

/**
 * @brief Create a new file on the simulated filesystem.
 * @param input_file Source file path.
 * @param simul_file Destination file path on the simulated file system.
 * @return 0 on success.
 */
int fs_create(char* input_file, char* simul_file){
		printf("teste1");
	int ret, size, qntSetores, directory, x;
	static FILE* simulfile =  NULL;
	int flag = 0;
	struct root_table_directory root_dir;
	struct table_directory table;
	struct sector_data sector,sectoraux;
	
	if ( (ret = ds_init(FILENAME, SECTOR_SIZE, NUMBER_OF_SECTORS, 0)) != 0 ){
		return ret;
	}

	/* Write the code to load a new file to the simulated filesystem. */


        //para criar o arquivo no file system
	printf("teste1");
	ds_read_sector(0, (void*)&root_dir, SECTOR_SIZE); //pego o conteudo do root	

            char str[60];
	    strcpy(str,simul_file);
	    char * nomeDiretorio;
	    char * nomeArquivo;
	    char * aux;		
	    x = 0;
            directory = 0;	
	    nomeDiretorio = strtok(str,"/");
  	    nomeArquivo = nomeDiretorio; // posso estar pisando em um arquivo de cara

	
	    while (1){		

	        aux = strtok (NULL, "/"); // vai um pra frente
		
		if(aux  == NULL) 
		{
		   break; 
		} else
		{
			directory = 1;
			nomeDiretorio = nomeArquivo;
			nomeArquivo = aux;
			if(flag == 0)
			{

				while(1)
				{
				if(strcmp(root_dir.entries[x].name,nomeDiretorio)){break;} x++;} flag = 1;ds_read_sector(root_dir.entries[x].sector_start, (void*)&table, SECTOR_SIZE);} // vejo se o diretorio está no root
			else
			{
			    while(1){if(strcmp(table.entries[x].name,nomeDiretorio)){break;} x++;} // vou entrando nos diretorios
			    ds_read_sector(table.entries[x].sector_start, (void*)&table, SECTOR_SIZE);
			}

			

		}
	    }

	
	printf("teste2");
	simulfile = fopen(input_file, "r"); // salvo no simul o inicio do arquivo
	
	if(simulfile == NULL)
	{
	   perror("fopen: ");
	}


	ds_read_sector(root_dir.free_sectors_list, (void*)&sector, SECTOR_SIZE); //pego o primeiro sector livre encontrado

	//descubro o tamanho do arquivo	
	fseek(simulfile,0,SEEK_END); // vou até o final do meu arquivo
	size = ftell(simulfile); //pego o valor desse ponteiro atual (que deve ser meu tamanho)
	fseek(simulfile,0,SEEK_SET); //volto para o início do arquivo		

	qntSetores = size/SECTOR_SIZE; // para saber quantos setores vou precisar

	if(directory == 1) // se tenho que atualizar a tabela de diretórios
	{

		int k = tableEntries;
	        if(k == 15) {k = 0; tableEntries = 0;}
		table.entries[k].dir = 0; // é um arquivo
		strcpy(table.entries[k].name,nomeArquivo);		
		table.entries[k].size_bytes = size; // coloco o tamanho
		table.entries[k].sector_start = root_dir.free_sectors_list; // digo a primeira posição
	
		ds_write_sector(table.entries[x].sector_start, (void*)&table, SECTOR_SIZE);

        int b = root_dir.free_sectors_list;
	int i;
	while(1)
	{
	  if( feof(simulfile) ){sector.next_sector = 0; break;}	

	  fread(&sector.data, sizeof(char), SECTOR_SIZE, simulfile); // adiciono tudo no meu setor
	  qntSetores--; //diminuio um da quantidade de setores que vou precisar
	  i = b;

	  if(qntSetores != 0) // caso tenha enchido o setor
	  {

		 while(b < NUMBER_OF_SECTORS){ds_read_sector(b, (void*)&sectoraux, SECTOR_SIZE);if(sectoraux.data == NULL){break;}b++;}  
  	
	     sector.next_sector = b;
             ds_write_sector(i, (void*)&sector, SECTOR_SIZE);	    
	     sector = sectoraux;	
	  }

	}
	b = 0;	

	while(b < NUMBER_OF_SECTORS){

		ds_read_sector(b, (void*)&sectoraux, SECTOR_SIZE);

		if(sectoraux.data == NULL) {break;}
	b++;
	}


	root_dir.free_sectors_list = b;
	
	} else
	{
 		if(directory == 0)
		{
			
 			int i = posEntries;
                	posEntries++;

			root_dir.entries[i].dir = 0; // é um arquivo
			strcpy(root_dir.entries[i].name,nomeArquivo);		
			root_dir.entries[i].size_bytes = size; // coloco o tamanho
			root_dir.entries[i].sector_start = root_dir.free_sectors_list; // digo a primeira posição livre onde vou armazenar meu arquivo

			        int b = root_dir.free_sectors_list;

	while(1)
	{

	  if( feof(simulfile) ){sector.next_sector = 0;break;}	

	  fread(&sector.data, sizeof(char), SECTOR_SIZE, simulfile); // adiciono tudo no meu setor
	  qntSetores--; //diminuio um da quantidade de setores que vou precisar
	  i = b;

	  if(qntSetores != 0)
	  {

		 while(b < NUMBER_OF_SECTORS){ds_read_sector(b, (void*)&sectoraux, SECTOR_SIZE);if(sectoraux.data == NULL){break;}
		
		b++;
	
	  }  
  	
	     sector.next_sector = b;
             ds_write_sector(i, (void*)&sector, SECTOR_SIZE);	    
	     sector = sectoraux;	
	  }


	b = 0;	

	while(b < NUMBER_OF_SECTORS){ds_read_sector(b, (void*)&sectoraux, SECTOR_SIZE);	if(sectoraux.data == NULL) {break;}b++;}


	root_dir.free_sectors_list = b;

	
		}
	}

	

	
}
	fclose(simulfile);
	
	ds_stop();
	
	return 0;
}


/**
 * @brief Read file from the simulated filesystem.
 * @param output_file Output file path.
 * @param simul_file Source file path from the simulated file system.
 * @return 0 on success.
 */
int fs_read(char* output_file, char* simul_file){
	int ret;
        FILE* simulfile;
	struct root_table_directory root_dir;
	struct table_directory table;
        struct sector_data sector;
	int flag = 0;
	int x;
    
	if ( (ret = ds_init(FILENAME, SECTOR_SIZE, NUMBER_OF_SECTORS, 0)) != 0 ){
		return ret;
	}
	
	/* Write the code to read a file from the simulated filesystem. */

            ds_read_sector(0, (void*)&root_dir, SECTOR_SIZE); //pego o conteudo do root	

            char str[60];
	    strcpy(str,simul_file);
	    char * nomeDiretorio;
	    char * nomeArquivo;
	    char * aux;		
	    nomeDiretorio = strtok(str,"/");
  	    nomeArquivo = nomeDiretorio; // posso estar pisando em um arquivo de cara

	
	    while (1){		

	        aux = strtok (NULL, "/"); // vai um pra frente
		
		if(aux  == NULL) 
		{
		   break; 
		} else
		{

			nomeDiretorio = nomeArquivo;
			nomeArquivo = aux;
			if(flag == 0)
			{
				x = 0;
				while(1)
				{
				if(strcmp(root_dir.entries[x].name,nomeDiretorio)){break;} x++;} flag = 1;ds_read_sector(root_dir.entries[x].sector_start, (void*)&table, SECTOR_SIZE);} // vejo se o diretorio está no root
			else
			{
			    x = 0;	
			    while(1){if(strcmp(table.entries[x].name,nomeDiretorio)){break;} x++;} // vou entrando nos diretorios
			    ds_read_sector(table.entries[x].sector_start, (void*)&table, SECTOR_SIZE);
			}

			

		}
	    }

	

	int i = 0;

	while(i < 15) { if(strcmp(table.entries[i].name, nomeArquivo)) {break;} i++;} // achei meu arquivo

	unsigned char * conteudoArquivo[table.entries[i].size_bytes]; //crio um array com o tamanho do meu arquivo

	ds_read_sector(table.entries[i].sector_start, (void*)&sector, SECTOR_SIZE); //pego o conteudo do primeiro setor 
	int contVetorFinal = 0;
	
	while(sector.next_sector != 0) // enquanto tiver setores para serem lidos
	{
		int k = 0;		
		while(k < 508){conteudoArquivo[contVetorFinal] = &sector.data[k]; k++; contVetorFinal++;}
		ds_read_sector(sector.next_sector, (void*)&sector, SECTOR_SIZE); //pego o conteudo do primeiro setor 

	}

	printf("%s\n",output_file);
	simulfile = fopen(output_file, "wb");
	
	fwrite(&conteudoArquivo, sizeof(char), SECTOR_SIZE, simulfile); // adiciono tudo no meu setor

        fclose(simulfile);
	
	ds_stop();
	
	return 0;
}

/**
 * @brief Delete file from file system.
 * @param simul_file Source file path.
 * @return 0 on success.
 */
int fs_del(char* simul_file){
	int ret;
	if ( (ret = ds_init(FILENAME, SECTOR_SIZE, NUMBER_OF_SECTORS, 0)) != 0 ){
		return ret;
	}
	
	/* Write the code delete a file from the simulated filesystem. */
	
	ds_stop();
	
	return 0;
}

/**
 * @brief List files from a directory.
 * @param simul_file Source file path.
 * @return 0 on success.
 */
int fs_ls(char *dir_path){
	int ret,x;
	struct root_table_directory root_dir;
	struct table_directory table;
        int flag;


	if ( (ret = ds_init(FILENAME, SECTOR_SIZE, NUMBER_OF_SECTORS, 0)) != 0 ){
		return ret;
	}
	
	/* Write the code to show files or directories. */
	
	ds_read_sector(0, (void*)&root_dir, SECTOR_SIZE); //pego o conteudo do root	

            char str[60];
	    strcpy(str,dir_path);
	    char * nomeDiretorio;
	    char * aux;		
	    x = 0;
            flag = 0;
	    nomeDiretorio = strtok(str,"/");

	
	    while (1){		

	        aux = strtok (NULL, "/"); // vai um pra frente
		
		if(aux  == NULL) 
		{
		   break; 
		} else
		{
			nomeDiretorio = aux;

			if(flag == 0)
			{

				while(1)
				{
				if(strcmp(root_dir.entries[x].name,nomeDiretorio)){break;} x++;} flag = 1;ds_read_sector(root_dir.entries[x].sector_start, (void*)&table, SECTOR_SIZE);} // vejo se o diretorio está no root
			else
			{
			    while(1){if(strcmp(table.entries[x].name,nomeDiretorio)){break;} x++;} // vou entrando nos diretorios
			    ds_read_sector(table.entries[x].sector_start, (void*)&table, SECTOR_SIZE);
			}

			

		}
	    }
		
		int k = 0;
		while (k < 15){printf("%s", table.entries[k].name); k++;}


	ds_stop();
	
	return 0;
}

/**
 * @brief Create a new directory on the simulated filesystem.
 * @param directory_path directory path.
 * @return 0 on success.
 */
int fs_mkdir(char* directory_path){
	int ret;


	if ( (ret = ds_init(FILENAME, SECTOR_SIZE, NUMBER_OF_SECTORS, 0)) != 0 ){
		return ret;
	}
	/* Write the code to create a new directory. */
	
	ds_stop();
	
	return 0;
}

/**
 * @brief Remove directory from the simulated filesystem.
 * @param directory_path directory path.
 * @return 0 on success.
 */
int fs_rmdir(char *directory_path){
	int ret;
	if ( (ret = ds_init(FILENAME, SECTOR_SIZE, NUMBER_OF_SECTORS, 0)) != 0 ){
		return ret;
	}
	
	/* Write the code to delete a directory. */
	
	ds_stop();
	
	return 0;
}

/**
 * @brief Generate a map of used/available sectors. 
 * @param log_f Log file with the sector map.
 * @return 0 on success.
 */
int fs_free_map(char *log_f){
	int ret, i, next;
	struct root_table_directory root_dir;
	struct sector_data sector;
	char *sector_array;
	FILE* log;
	int pid, status;
	int free_space = 0;
	char* exec_params[] = {"gnuplot", "sector_map.gnuplot" , NULL};

	if ( (ret = ds_init(FILENAME, SECTOR_SIZE, NUMBER_OF_SECTORS, 0)) != 0 ){
		return ret;
	}
	
	/* each byte represents a sector. */
	sector_array = (char*)malloc(NUMBER_OF_SECTORS);

	/* set 0 to all sectors. Zero means that the sector is used. */
	memset(sector_array, 0, NUMBER_OF_SECTORS);
	
	/* Read the root dir to get the free blocks list. */
	ds_read_sector(0, (void*)&root_dir, SECTOR_SIZE);
	
	next = root_dir.free_sectors_list;

	while(next){
		/* The sector is in the free list, mark with 1. */
		sector_array[next] = 1;
		
		/* move to the next free sector. */
		ds_read_sector(next, (void*)&sector, SECTOR_SIZE);
		
		next = sector.next_sector;
		
		free_space += SECTOR_SIZE;
	}

	/* Create a log file. */
	if( (log = fopen(log_f, "w")) == NULL){
		perror("fopen()");
		free(sector_array);
		ds_stop();
		return 1;
	}
	
	/* Write the the sector map to the log file. */
	for(i=0;i<NUMBER_OF_SECTORS;i++){
		if(i%32==0) fprintf(log, "%s", "\n");
		fprintf(log, " %d", sector_array[i]);
	}
	
	fclose(log);
	
	/* Execute gnuplot to generate the sector's free map. */
	pid = fork();
	if(pid==0){
		execvp("gnuplot", exec_params);
	}
	
	wait(&status);
	
	free(sector_array);
	
	ds_stop();
	
	printf("Free space %d kbytes.\n", free_space/1024);
	
	return 0;
}

