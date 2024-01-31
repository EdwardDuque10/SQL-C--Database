#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>

#include <mysql/jdbc.h>

//#include <conio.h>

using namespace std;

// reads in a password without echoing it to the screen
string myget_passwd()
{
	string passwd;
	cin >> passwd;
	return passwd;
}

int main()
{

	// strings for mysql hostname, userid, ...
	string db_host;
	string db_port;
	string db_user;
	string db_password;
	string db_name;

	// set up the client (this machine) to use mysql
	cout << "initializing client DB subsystem ..."; cout.flush();
	//mysql_init(&mysql);
	sql::Driver * driver = sql::mysql::get_driver_instance();
	cout << "Done!" << endl;

	// get user credentials and mysql server info
	cout << "Enter MySQL database hostname (or IP adress):";
	getline(cin, db_host); // cin >> db_host;

	cout << "Enter MySQL database port number (press enter for default):";
	getline(cin, db_port);
	if (db_port.length() > 0)
		db_host += ":" + db_port;

	cout << "Enter MySQL database username:";
	cin >> db_user;

	cout << "Enter MySQL database password:";
	db_password = myget_passwd();

	// could also prompt for this, if desired
	db_name = db_user;

	sql::Connection* conn= nullptr;

	try
	{
		//Connect to MySQL database
		conn = driver->connect(db_host, db_user, db_password);
		conn->setSchema(db_name);

	} catch (sql::SQLException sqle){
		//handle exception if connection fails
		cout << "Couldn't connect to the database"<< sqle.what()<< endl;
	}

	//variables for the Book.
    string isbn;
    int year = 0;
    string author;
    string title;

    //variables for the Library.
    string libraryName;
    string city;
    int zipCode;

    //boolean variable used to exit program.
    bool exitProgram= false;

    //while "exitProgram" is not true, keep asking the user to enter commands and variables.
    while(!exitProgram){

        cout<<">>> ";
        char command; //variable used for the switch statements.
        cin>>command;



        switch(command){

			case 'a':{
				//used for subcommands under 'a'
				cin>> command;

				switch(command){

					case 'b':{

						cin>>isbn >> year >> author;
						getline(cin, title);

						//MySQL statement and result set objects.
						sql::Statement *stmnt= nullptr;
						stmnt= conn->createStatement();
						sql::ResultSet *result= nullptr;

						//executes query to check if the book with the ISBN provided already exists.
						result= stmnt->executeQuery("SELECT * FROM books where isbn= '"+ isbn + "'");

						if(result->next()){
						cout<<"ISBN already exists in the database"<< endl;

						} else{//if the book doesn't exists, insert it to the books table
						stmnt->execute("INSERT INTO books (isbn, year, author, title) VALUES ('"+ isbn + "'," + to_string(year) + ",'"+ author + "','"+ title + "')");

						cout<<"Book added to Database"<< endl;
						}

						//Release objects.
						delete result;
						delete stmnt;

						break;

					}

					case 'l':{

						cin>>libraryName >>city >>zipCode;
						sql::Statement *stmnt= nullptr;
						stmnt= conn->createStatement();
						sql::ResultSet *result= nullptr;

						//checks if the library already exists.
						result= stmnt->executeQuery("SELECT * FROM Libraries WHERE Library = '" + libraryName + "'");

						if(result->next()){
							//message to indicate the Library already exists in the database.
							cout<<"Library already exists in the database"<<endl;
						}else{
							//if it doesn't exists, insert it into the Libraries table.
							stmnt->execute("INSERT INTO Libraries (Library, City, ZipCode) VALUES ('" + libraryName + "', '" + city + "', " + to_string(zipCode) + ")");
							cout<<"Library added to the database"<< endl;
						}

						delete result;
						delete stmnt;

						break;

					}

					case 'h':{

						cin>>isbn >>libraryName;

						sql::Statement *stmnt= nullptr;
						stmnt= conn->createStatement();
						sql::ResultSet *result= nullptr;

						//checks if the book is already in the Library.
						result= stmnt->executeQuery("SELECT * FROM BookLibrary WHERE isbn = '" + isbn + "' AND Library = '" + libraryName + "'");

						if(result->next())
						{	//if it is in the Library, update the copy number.
							int copyNumber= result->getInt("CopyNumber")+ 1;

							stmnt->execute("UPDATE BookLibrary SET CopyNumber = " + to_string(copyNumber) + " WHERE isbn = '" + isbn + "' AND Library = '" + libraryName + "'");
							cout<<"Book added to the Library. Copy Number: "<< copyNumber<< endl;

						} else{
							//if it's not in the library, add it as the first copy.
							stmnt->execute("INSERT INTO BookLibrary (isbn, Library, CopyNumber) VALUES ('" + isbn + "', '" + libraryName + "', 1)");
							cout << "Book added to the library. Copy number: 1" << endl;

						}

						delete result;
						delete stmnt;

						break;
					}

				}

				case 'l':{

					cin>> command;

					switch(command){

						case 'b':{

							sql::Statement *stmnt= nullptr;
							stmnt= conn->createStatement();
							sql::ResultSet *result= nullptr;

							//Selects the list of books from the database
							result= stmnt->executeQuery("SELECT * FROM books");
							cout<<"Books in the database:"<<endl;

							//iterates through each result and prints the information.
							while(result->next()){

								cout << result->getString("isbn") <<" " << result->getInt("year")
                        		<< " "<< result->getString("author") <<" "<< result->getString("title") << endl;
							}

							delete result;
							delete stmnt;

							break;

						}

						case 'l':{

							sql::Statement *stmnt= nullptr;
							stmnt= conn->createStatement();
							sql::ResultSet *result= nullptr;

							//Select the libraries from the database
							result= stmnt->executeQuery("SELECT * FROM Libraries");
							cout<<"Libraries in the database:"<< endl;

							//iterates through each result and prints the information.
							while(result->next()){

								cout<<result->getString("Library")<<" "<<result->getString("City")<<" "
								<<result->getInt("ZipCode")<< endl;

							}

							delete result;
							delete stmnt;

							break;

						}

					}

					break;
				}

				case 'f':{

					cin>> isbn;
					sql::Statement *stmnt= nullptr;
					stmnt= conn->createStatement();
					sql::ResultSet *result= nullptr;

					//query to find the book with the given ISBN.
					result= stmnt->executeQuery("SELECT * FROM BookLibrary "
                                "JOIN books ON BookLibrary.ISBN = books.isbn "
                                "JOIN Libraries ON BookLibrary.Library = Libraries.Library "
                                "WHERE BookLibrary.isbn = '" + isbn + "'");

					//iterate through each result and prints the information.
					while(result->next()){

						cout<< result->getString("Library")<<" "<<result->getString("isbn")<<" "<<result->getInt("year")<<" "
						<<result->getString("author")<<" "<<result->getString("title")<<" "<<result->getInt("CopyNumber")<<endl;
					}

					delete result;
					delete stmnt;

					break;


				}

				case 'd':
				{
					cin>> isbn>> libraryName;

					sql::Statement *stmnt= nullptr;
					stmnt= conn->createStatement();
					//executes a query that deletes the book with the given ISBN and Library.
					stmnt->execute("DELETE FROM BookLibrary WHERE ISBN = '" + isbn + "' AND Library = '" + libraryName + "'");

					delete stmnt;

					break;

				}

				case 'q':
				{
					exitProgram= true;
				}


		}	}

	}

	return 0;
}
