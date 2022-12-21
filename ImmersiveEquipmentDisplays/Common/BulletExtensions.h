#pragma once

namespace Bullet
{
	class btVector3Ex : public btVector3
	{
	public:
		using btVector3::btVector3;
		using btVector3::operator=;

		btVector3Ex() = default;

		SIMD_FORCE_INLINE btVector3Ex(
			const NiPoint3& a_pos) noexcept
		{
			setValue(a_pos.x, a_pos.y, a_pos.z);
		}

		SIMD_FORCE_INLINE btVector3Ex(
			const NiTransform& a_tf) noexcept
		{
			mVec128 = _mm_and_ps(_mm_loadu_ps(a_tf.pos), btvFFF0fMask);
		}

		SIMD_FORCE_INLINE operator NiPoint3&() noexcept { return reinterpret_cast<NiPoint3&>(mVec128); }
		SIMD_FORCE_INLINE operator const NiPoint3&() const noexcept { return reinterpret_cast<const NiPoint3&>(mVec128); }
		SIMD_FORCE_INLINE operator NiPoint3*() noexcept { return reinterpret_cast<NiPoint3*>(mVec128.m128_f32); }
		SIMD_FORCE_INLINE operator const NiPoint3*() const noexcept { return reinterpret_cast<const NiPoint3*>(mVec128.m128_f32); }
	};

	class btTransformEx : public btTransform
	{
		btScalar m_scale;

	public:
		using btTransform::btTransform;
		using btTransform::operator=;

		SIMD_FORCE_INLINE btTransformEx(
			const NiTransform& a_tf) noexcept
		{
			_copy_from_nitransform(a_tf);
		}

		SIMD_FORCE_INLINE btTransformEx(
			NiTransform&& a_tf) noexcept
		{
			_copy_from_nitransform(a_tf);
		}

		SIMD_FORCE_INLINE btTransformEx& operator=(
			const NiTransform& a_tf) noexcept
		{
			_copy_from_nitransform(a_tf);
			return *this;
		}

		SIMD_FORCE_INLINE btTransformEx& operator=(
			NiTransform&& a_tf) noexcept
		{
			_copy_from_nitransform(a_tf);
			return *this;
		}

		SIMD_FORCE_INLINE btTransformEx operator*(
			const btTransformEx& a_rhs) const noexcept
		{
			btTransformEx result;

			result.m_scale       = m_scale * a_rhs.m_scale;
			result.getRotation() = getRotation() * a_rhs.getRotation();
			result.getBasis()    = getBasis() * a_rhs.getBasis();

			return result;
		}

		SIMD_FORCE_INLINE btVector3 operator*(
			const btVector3& a_pt) const noexcept
		{
			return ((getBasis() * a_pt) *= m_scale) += getOrigin();
		}

		SIMD_FORCE_INLINE btScalar getScale() const noexcept
		{
			return m_scale;
		}

		SIMD_FORCE_INLINE void setScale(btScalar a_scale) noexcept
		{
			m_scale = a_scale;
		}

		SIMD_FORCE_INLINE NiTransform getNiTransform() const noexcept
		{
			NiTransform result(NiTransform::noinit_arg_t{});

			_write_to_nitransform(result);

			return result;
		}

		SIMD_FORCE_INLINE void writeNiTransform(NiTransform& a_tf) const noexcept
		{
			_write_to_nitransform(a_tf);
		}

	private:
		SIMD_FORCE_INLINE void _copy_from_nitransform(
			const NiTransform& a_tf) noexcept
		{
			auto& b = getBasis();

			b[0].set128(_mm_and_ps(_mm_loadu_ps(a_tf.rot.data[0]), btvFFF0fMask));
			b[1].set128(_mm_and_ps(_mm_loadu_ps(a_tf.rot.data[1]), btvFFF0fMask));
			b[2].set128(_mm_and_ps(_mm_loadu_ps(a_tf.rot.data[2]), btvFFF0fMask));

			getOrigin().set128(_mm_and_ps(_mm_loadu_ps(a_tf.pos), btvFFF0fMask));

			m_scale = a_tf.scale;
		}

		SIMD_FORCE_INLINE void _write_to_nitransform(
			NiTransform& a_tf) const noexcept
		{
			static_assert(offsetof(NiTransform, rot) == 0x0);
			static_assert(offsetof(NiTransform, pos) == 0x24);
			static_assert(offsetof(NiTransform, scale) == 0x30);
			static_assert(sizeof(NiTransform) == 0x34);

			auto& b = getBasis();

			_mm_storeu_ps(a_tf.rot.data[0], b[0].get128());
			_mm_storeu_ps(a_tf.rot.data[1], b[1].get128());
			_mm_storeu_ps(a_tf.rot.data[2], b[2].get128());

			_mm_storeu_ps(a_tf.pos, getOrigin().get128());

			a_tf.scale = m_scale;
		}
	};
}