using namespace me;

template temp_string<CHAR_T>::temp_string(size_t length, const CHAR_T* str);
template temp_string<CHAR_T>::temp_string(size_t length, const CHAR_T c);
template temp_string<CHAR_T>::temp_string(const CHAR_T* str);
template temp_string<CHAR_T>::temp_string(const temp_string<CHAR_T> &copy);
template temp_string<CHAR_T>::temp_string();
template temp_string<CHAR_T>::~temp_string();

template CHAR_T* temp_string<CHAR_T>::data() const;
template CHAR_T* temp_string<CHAR_T>::begin() const;
template CHAR_T* temp_string<CHAR_T>::end() const;
template void temp_string<CHAR_T>::reserve(size_t bytes);

template void temp_string<CHAR_T>::assign(size_t len, const CHAR_T* str);
template void temp_string<CHAR_T>::assign(const temp_string<CHAR_T> &str);
template void temp_string<CHAR_T>::append(size_t len, const CHAR_T* str);
template void temp_string<CHAR_T>::append(const temp_string<CHAR_T> &str);
template void temp_string<CHAR_T>::append(const CHAR_T c);
template CHAR_T temp_string<CHAR_T>::pop_back();
template void temp_string<CHAR_T>::erase(CHAR_T* first, CHAR_T* last);
template void temp_string<CHAR_T>::erase(CHAR_T* pos);
template void temp_string<CHAR_T>::replace(size_t first, size_t last, size_t len, const CHAR_T* str);
template void temp_string<CHAR_T>::replace(size_t first, size_t last, const temp_string<CHAR_T> &str);
template void temp_string<CHAR_T>::to_lowercase(size_t off = 0, size_t len = -1);
template void temp_string<CHAR_T>::to_uppercase(size_t off = 0, size_t len = -1);

template CHAR_T& temp_string<CHAR_T>::at(size_t index) const;

template size_t temp_string<CHAR_T>::find(const CHAR_T c) const;
template size_t temp_string<CHAR_T>::rfind(const CHAR_T c) const;
template size_t temp_string<CHAR_T>::find(size_t len, const CHAR_T* str) const;
template size_t temp_string<CHAR_T>::rfind(size_t len, const CHAR_T* str) const;
template size_t temp_string<CHAR_T>::find(const temp_string<CHAR_T> &str) const;
template size_t temp_string<CHAR_T>::rfind(const temp_string<CHAR_T> &str) const;
template temp_string<CHAR_T> temp_string<CHAR_T>::substr(size_t begin, size_t end) const;
template temp_string<CHAR_T> temp_string<CHAR_T>::substr(size_t begin) const;
template void temp_string<CHAR_T>::copy(size_t off, size_t len, CHAR_T* dest) const;

template const CHAR_T* temp_string<CHAR_T>::c_str() const;
template size_t temp_string<CHAR_T>::capacity() const;
template size_t temp_string<CHAR_T>::size() const;
template bool temp_string<CHAR_T>::is_empty() const;

template temp_string<CHAR_T>& temp_string<CHAR_T>::operator+=(const CHAR_T c);
template temp_string<CHAR_T>& temp_string<CHAR_T>::operator+=(const temp_string<CHAR_T> &str);
template temp_string<CHAR_T>& temp_string<CHAR_T>::operator*=(size_t count);

template CHAR_T& temp_string<CHAR_T>::operator[](size_t index) const;

template bool temp_string<CHAR_T>::operator==(const temp_string<CHAR_T> &str) const;
template bool temp_string<CHAR_T>::operator!=(const temp_string<CHAR_T> &str) const;
template bool temp_string<CHAR_T>::operator>(const temp_string<CHAR_T> &str) const;
template bool temp_string<CHAR_T>::operator<(const temp_string<CHAR_T> &str) const;
template bool temp_string<CHAR_T>::operator>=(const temp_string<CHAR_T> &str) const;
template bool temp_string<CHAR_T>::operator<=(const temp_string<CHAR_T> &str) const;
