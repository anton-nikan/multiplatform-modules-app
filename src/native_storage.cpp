export module native_storage;

import std;

// Inline storage for the platform-specific part of a context. The interfaces only
// reserve space; the concrete type is named in the `*.native` modules alone, so the
// generic code never imports (or even reaches) anything platform-specific.
// Unique address per type, used instead of RTTI to check that the stored type matches.
// Deliberately a mutable object: distinct objects are guaranteed distinct addresses,
// unlike constants, which the toolchain may merge.
template<typename T>
inline char type_tag = 0;

export template<std::size_t Capacity, std::size_t Alignment = alignof(std::max_align_t)>
class native_storage {
	alignas(Alignment) std::byte buffer_[Capacity];
	const void* type_ = nullptr;
	void (*destroy_)(void*) = nullptr;
public:
	native_storage() = default;
	native_storage(const native_storage&) = delete;
	native_storage& operator=(const native_storage&) = delete;
	~native_storage() { reset(); }

	template<typename Native, typename... Args>
	Native& emplace(Args&&... args) {
		static_assert(sizeof(Native) <= Capacity, "native part does not fit, increase the capacity in the interface");
		static_assert(alignof(Native) <= Alignment, "native part is over-aligned, increase the alignment in the interface");
		reset();
		Native* p = ::new (static_cast<void*>(buffer_)) Native(std::forward<Args>(args)...);
		type_ = &type_tag<Native>;
		destroy_ = [](void* q) { static_cast<Native*>(q)->~Native(); };
		return *p;
	}

	template<typename Native>
	Native& as() {
		if (type_ != &type_tag<Native>) {
			throw std::logic_error("native part is not created or has a different type");
		}
		return *std::launder(reinterpret_cast<Native*>(buffer_));
	}

	bool has_value() const noexcept {
		return destroy_ != nullptr;
	}

	void reset() {
		if (destroy_ != nullptr) {
			destroy_(buffer_);
			destroy_ = nullptr;
			type_ = nullptr;
		}
	}
};
