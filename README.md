## About
Local Communications Network is a single-threaded server that allows creation of accounts by use of display names. </br>
Names are stored via plaintext in MySQL databases which can be programmatically interacted with using the API. </br>

## Prerequisites
If you are hosting this server locally on 127.0.0.1 (the default), you need to install MySQL Server version >= 8.0.30. </br>
You can find the downloads for that [here](https://dev.mysql.com/downloads/). </br>
The MySQL C API documentation can be found [here](https://dev.mysql.com/doc/c-api/8.0/en/c-api-basic-interface.html). </br>

## Building
The easiest way to build this is to install [Visual Studio Community 2022](https://visualstudio.microsoft.com/downloads/). </br>
You can also try [Visual Studio Code](https://code.visualstudio.com/docs/cpp/config-msvc) with the MSVC compiler toolset. </br>
Make sure to add the additional include directory for `<mysql.h>`. The default install path should be in `C:\Program Files\MySQL\`. </br>

## Usage
The MySQL API in LCN currently only supports existing database manipulation. </br>
There are four database functions: </br>
```cpp
void CreateTable(const std::string& tbName, const std::string& colName);
void DropTable(const std::string& tbName);
void InsertKey(const std::string& tbName, const std::string& colName, const std::string& idxData);
void QueryTable(const std::string& tbName, const int& idxPos);
```
Each function assumes you already initialized the database via the member initializer list. </br>
- `CreateTable` uses `varchar(255)` for historical and performance reasons. [Read more](https://stackoverflow.com/questions/1217466)
- `QueryTable` will automatically log the request response to the console.

## License
This project is licensed under [MIT License](https://github.com/eepsite/lcn/blob/master/LICENSE). Learn more [here](https://choosealicense.com/licenses/mit/).
