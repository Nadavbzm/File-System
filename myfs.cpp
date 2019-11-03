#include "myfs.h"
#include <string.h>
#include <iostream>
#include <math.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

const char *MyFs::MYFS_MAGIC = "MYFS";

MyFs::MyFs(BlockDeviceSimulator *blkdevsim_):blkdevsim(blkdevsim_) {
	struct myfs_header header;
	blkdevsim->read(0, sizeof(header), (char *)&header);

	if (strncmp(header.magic, MYFS_MAGIC, sizeof(header.magic)) != 0 ||
	    (header.version != CURR_VERSION)) {
		std::cout << "Did not find myfs instance on blkdev" << std::endl;
		std::cout << "Creating..." << std::endl;
		std::cout << "Finished!" << std::endl;
		format();
	}
}

void MyFs::format()
{

	// put the header in place
	struct myfs_header header;
	strncpy(header.magic, MYFS_MAGIC, sizeof(header.magic));
	header.version = CURR_VERSION;
	blkdevsim->write(0, sizeof(header), (const char*)&header);

	// TODO: put your format code here
}

void MyFs::create_file(std::string path_str, bool directory)
{
	char fileNumChar[2] = "";
	char checkFileNum[2] = "";
	blkdevsim->read(FILE_NUM_OFFSET, 1, checkFileNum);
	if(!(strcmp(fileNumChar, checkFileNum)))
	{
		file_num = 1;
		fileNumChar[0] = '1';
		blkdevsim->write(FILE_NUM_OFFSET, 1, "1"); //writing file num
		//std::cout << "wrote num of files into system: " << checkFileNum << "\n";
	}
	else
	{
		//std::cout << "Got different results, num of files saved to system" << "\n";
		fileNumChar[0] = checkFileNum[0];
		file_num = atoi(fileNumChar);
	}

//******************
	struct dir_list_entry tempFile;
	strncpy(tempFile.name_c, path_str.c_str(), NAME_OFFSET);
//********************
	tempFile.is_dir = directory;
	tempFile.addr = FIRST_OFFSET + (file_num * FILE_SIZE);
	tempFile.file_size = FILE_SIZE;
	blkdevsim->write(tempFile.addr, path_str.size(), tempFile.name_c); //WRITING NAME
	char checkName[10] = "";
	blkdevsim->read(tempFile.addr, NAME_OFFSET, checkName);
	//std::cout << "file name: " << checkName << "\n";
//****************
	tempFile.name = tempFile.name_c;
	//std::cout << "offset of " <<  path_str << " is: " << tempFile.addr << "\n";
	//std::cout << "num of files " <<  fileNumChar << "\n";

	file_num++;
	snprintf(fileNumChar, 2, "%d", file_num);
	blkdevsim->write(FILE_NUM_OFFSET, 1, fileNumChar);
}

std::string MyFs::get_content(std::string path_str) {

	/* Getting number of files*/
	char fileNumChar[1];
	blkdevsim->read(FILE_NUM_OFFSET, 1, fileNumChar); //writing file num
	int fileNum = atoi(fileNumChar);
	//std::cout << "num of files " <<  fileNum << "\n";

	/*comparison and address variables*/
	int offset = FIRST_OFFSET; //OFFSET OF FIRST FILE
	char CurrName[10] = ""; //name buffer for comparing

	char fileData[FILE_SIZE]; //Data of file

	for (int i = 0; i < fileNum; i++)
	{
		blkdevsim->read(offset, NAME_OFFSET, CurrName);//getting name in current memory
		//std::cout << "current offset: " << offset << "\n";
		//std::cout << "current file name: " << CurrName << "\n";

		if(!strcmp(path_str.c_str(), CurrName))
		{
			blkdevsim->read(offset+NAME_OFFSET, FILE_SIZE-NAME_OFFSET, fileData);
			return fileData;
		}
		offset += FILE_SIZE;
	}

	return fileData;
}

void MyFs::set_content(std::string path_str, std::string content) {

	/* Getting number of files*/
	char fileNumChar[1];
	blkdevsim->read(FILE_NUM_OFFSET, 1, fileNumChar); //reading file num
	int fileNum = atoi(fileNumChar);
	//std::cout << "num of files " <<  fileNum << "\n";

	/*comparison and address variables*/
	int offset = FIRST_OFFSET; //OFFSET OF FIRST FILE
	char CurrName[10] = ""; //name buffer for comparing

	for (int i = 0; i < fileNum; i++)
	{
		blkdevsim->read(offset, NAME_OFFSET, CurrName);//getting name in current memory
		//std::cout << "current offset: " << offset << "\n";
		//std::cout << "current file name: " << CurrName << "\n";

		if(!strcmp(path_str.c_str(), CurrName))
		{
			blkdevsim->write(offset+NAME_OFFSET, FILE_SIZE-NAME_OFFSET, content.c_str());
			//std::cout << "Wrote to file!-----------------\n";
		}
		offset += FILE_SIZE;
	}
}

MyFs::dir_list MyFs::list_dir(std::string path_str)
{

	dir_list list;
	/* Getting number of files*/
	char fileNumChar[1];
	blkdevsim->read(FILE_NUM_OFFSET, 1, fileNumChar); //reading file num
	int fileNum = atoi(fileNumChar);
	//std::cout << "num of files " <<  fileNum << "\n";

	/*comparison and address variables*/
	int offset = FIRST_OFFSET; //OFFSET OF FIRST FILE
	char CurrName[10] = ""; //name buffer for comparing

	for (int i = 0; i < fileNum; i++)
	{
		blkdevsim->read(offset, NAME_OFFSET, CurrName);//getting name in current memory

		dir_list_entry tempFile;
		tempFile.addr = offset;
		tempFile.file_size = FILE_SIZE;
		tempFile.name = CurrName;
		list.push_back(tempFile);
		offset += FILE_SIZE;
	}
	return list;
}
