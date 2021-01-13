using namespace me;

template temp_string_view<CHAR_T>::temp_string_view(const size_t length, const CHAR_T* str);
template temp_string_view<CHAR_T>::temp_string_view(const CHAR_T* str);
template temp_string_view<CHAR_T>::temp_string_view(const temp_string_view<CHAR_T> &str_view);
template temp_string_view<CHAR_T>::temp_string_view();

template size_t temp_string_view<CHAR_T>::find(const CHAR_T c) const;
template size_t temp_string_view<CHAR_T>::rfind(const CHAR_T c) const;
template size_t temp_string_view<CHAR_T>::find(const size_t len, const CHAR_T* c) const;
template size_t temp_string_view<CHAR_T>::rfind(const size_t len, const CHAR_T* c) const;
template size_t temp_string_view<CHAR_T>::find(const temp_string_view<CHAR_T> &str) const;
template size_t temp_string_view<CHAR_T>::rfind(const temp_string_view<CHAR_T> &str) const;
template void temp_string_view<CHAR_T>::copy(size_t off, size_t len, CHAR_T* dest) const;

template const CHAR_T* temp_string_view<CHAR_T>::c_str(CHAR_T* str) const;
template size_t temp_string_view<CHAR_T>::size() const;
template bool temp_string_view<CHAR_T>::is_empty() const;

template const CHAR_T temp_string_view<CHAR_T>::operator[](size_t index) const;

template bool temp_string_view<CHAR_T>::operator==(const temp_string_view<CHAR_T> &str) const;
template bool temp_string_view<CHAR_T>::operator!=(const temp_string_view<CHAR_T> &str) const;
template bool temp_string_view<CHAR_T>::operator>(const temp_string_view<CHAR_T> &str) const;
template bool temp_string_view<CHAR_T>::operator<(const temp_string_view<CHAR_T> &str) const;
template bool temp_string_view<CHAR_T>::operator>=(const temp_string_view<CHAR_T> &str) const;
template bool temp_string_view<CHAR_T>::operator<=(const temp_string_view<CHAR_T> &str) const;
