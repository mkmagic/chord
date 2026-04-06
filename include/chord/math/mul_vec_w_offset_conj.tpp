namespace chord::math {

template <typename E>
auto mul_vec_w_offset_conj(const E&& input, size_t offset, Status& status, size_t trunc_start, size_t trunc_end) {
    auto length = input.size() - offset - trunc_end - trunc_start;
    if (length <= 0) {
        status = Status::INPUT_TOO_SMALL;
        return {0.0f, 0.0f};
    }
    
    auto input1 = input.slice(trunc_start, length);
    auto input2 = input.slice(offset + trunc_start, length);
    
    status = Status::OK;
    return input1 * kfr::cconj(input2);
}

}  // namespace chord::math
