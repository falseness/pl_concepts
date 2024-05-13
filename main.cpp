#include "macro.hpp"

// Функция для обработки ситуации повторного исключения
void crash(int first, int second) {
    std::cout << "In custom crash! " << first << ' ' << second << std::endl;
    std::exit(1);
}

// Функция бросает исключение
int divide( int num, int denum ) {
	if( denum == 0	) {
		THROW(error::math_error);
	}
	return num / denum;
}

struct CMyClass {
    int GetValue() { return 1; }
    ~CMyClass() {std::cout <<"CMyClass destructor" << std::endl;}
};

struct CMyClass2 {
    ~CMyClass2() {std::cout <<"CMyClass2 destructor" << std::endl;}
};

// сквозь эту функцию пролетает исключение, вылетающее из `divide`
void auxiliary()
{
	AUTO_OBJECT( CMyClass, auxiliaryObject );
	int z = divide( auxiliaryObject.GetValue(), 0 );
}

void SimpleTest() {
	TRY {
		// Разместите объект, у которого должен быть вызван деструктор
		AUTO_OBJECT( CMyClass, resourceObject );
		// Эта функция бросит исключение
		auxiliary();

	} CATCH( error::io_error ) {
		std::cout << "IO ERROR" << std::endl;
		
	} CATCH( error::math_error ) {
		std::cout << "MATH ERROR" << std::endl;
	}
}

void NestedThrowsTest() {
    TRY {
        AUTO_OBJECT( CMyClass2, resourceObject );
        SimpleTest();
        THROW(error::io_error);
    }
    CATCH( error::io_error ) {
		std::cout << "OUTER!! IO ERROR" << std::endl;
		
	} CATCH( error::math_error ) {
		std::cout << "OUTER!! MATH ERROR" << std::endl;
	}
}

struct ThrowsInDestructorObject {
    ~ThrowsInDestructorObject() {THROW(error::io_error);}
};

void CrashTest() {
    TRY {
        AUTO_OBJECT( ThrowsInDestructorObject, resourceObject );
        THROW(error::math_error);
    }
    CATCH( error::io_error ) {
		std::cout << "IO ERROR" << std::endl;
		
	} CATCH( error::math_error ) {
		std::cout << "MATH ERROR" << std::endl;
	}
}

int main() {
    std::cout << "SimpleTest" << std::endl;
    SimpleTest();
    std::cout << "\nNestedThrowsTest" << std::endl;
    NestedThrowsTest();
    std::cout << "\nCrash custom" << std::endl;
    SET_UNEXPECTED_HANDLER(crash);
    CrashTest();
}
