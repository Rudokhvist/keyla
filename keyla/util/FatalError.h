//
// Вспомогательный модуль FATAL_ERROR предоставляет структуру FatalError
//

#pragma once

#include <exception>

// Исключение, которое следует выбросить в случае неисправимой ошибки,
// препятствующей нормальному функционированию программы. Исключение
// будет перехвачено в модуле MAIN, который выведет окно предупреждения
class FatalError : public std::exception {
public:

	// Получить строковое описание последней ошибки в подсистеме Win32 (см. GetLastError)
	static tstring lastWin32Error();

	// Конструктор
	// message - описание ошибки
	FatalError(const tstring & message) : m_message(message) {
	}

	// Получить описание ошибки
	const tstring & message() const {
		return m_message;
	}

private:

	// Описание ошибки
	const tstring m_message;
};


