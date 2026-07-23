// Copyright 2020-2023 Bloomberg Finance L.P.
// SPDX-License-Identifier: Apache-2.0
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_NTSA_CIRCULAR
#define INCLUDED_NTSA_CIRCULAR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bslh_hash.h>
#include <bsl_cstddef.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntsa {

/// Describes 16-bit unsigned circular integer.
///
/// @details
/// Like fundamental two's complement unsigned integers, a circular integer is
/// permitted to "wrap around" when it reaches its minumum or maximum value.
/// But unlike fundamental two's complement unsigned integers, a circular
/// integer still maintains a valid definition of comparison and arithmetic
/// operators even after wrap-around occurs, provided one integer has
/// wrapped-around only one more time than the other.
///
/// These semantics are commonly required in sequence numbers used in
/// communications protocols.
///
/// For any two values stored there are two possible gaps between them. For
/// example, if one value is '5' and the other value is 'USHRT_MAX - 10' then
/// the gap is either '15' or 'USHRT_MAX - 15'.  All comparisons and arithmetic
/// performed by this type assume that the correct gap is the smaller of the
/// two possible gaps.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_data
class CircularUint16
{
  public:
    /// Defines a type alias for the value type.
    typedef bsl::uint16_t value_type;

    /// Defines a type alias for the difference type.
    typedef bsl::int16_t difference_type;

    /// Create a new 16-bit circular unsigned integer having the default value.
    CircularUint16();

    /// Create a new 16-bit circular unsigned integer having the specified
    /// 'value'.
    explicit CircularUint16(bsl::uint16_t value);

    /// Create a new 16-bit circular unsigned integer having the same value as
    /// the specified 'original' object. Assign an unspecified but valid value
    /// to the 'original' original.
    CircularUint16(bslmf::MovableRef<CircularUint16> original) NTSCFG_NOEXCEPT;

    /// Create a new 16-bit circular unsigned integer having the same value as
    /// the specifid 'original' value.
    CircularUint16(const CircularUint16& original);

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    CircularUint16& operator=(bslmf::MovableRef<CircularUint16> other)
        NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    CircularUint16& operator=(const CircularUint16& other);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    CircularUint16& operator=(bsl::uint16_t value);

    /// Increment the value of this object by one and return a reference to
    /// this modifiable object.
    CircularUint16& operator++();

    /// Increment the value of this object by one and return a copy of this
    /// object having the value of this object before it was incremented.
    CircularUint16 operator++(int);

    /// Decrement the value of this object by one and return a reference to
    /// this modifiable object.
    CircularUint16& operator--();

    /// Decrement the value of this object by one and return a copy of this
    /// object having the value of this object before it was decremented.
    CircularUint16 operator--(int);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Assign the specified 'value' to this object.
    void assign(bsl::uint16_t value);

    /// Increment the value by the specified 'delta'.
    void increment(bsl::uint16_t delta);

    /// Decrement the value by the specified 'delta'.
    void decrement(bsl::uint16_t delta);

    /// Return the value.
    bsl::uint16_t value() const;

    /// Return the signed difference between this object and the specified
    /// 'other' object. Note that if this object is greater than the 'target',
    /// the result is positive, if this object is less than the 'other' object
    /// the result is negative, and if the two objects have the same value the
    /// result is zero.
    bsl::int16_t difference(const CircularUint16& other) const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const CircularUint16& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const CircularUint16& other) const;

    /// Return true if the value of this object is less than or equal to the
    /// value of the specified 'other' object, otherwise return false.
    bool lessOrEquals(const CircularUint16& other) const;

    /// Contribute the values of the salient attributes of this object to the
    /// specified hash 'algorithm'.
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm) const;

    /// Format this object to the specified output 'stream' at the optionally
    /// specified indentation 'level' and return a reference to the modifiable
    /// 'stream'.  If 'level' is specified, optionally specify
    /// 'spacesPerLevel', the number of spaces per indentation level for this
    /// and all of its nested objects.  Each line is indented by the absolute
    /// value of 'level * spacesPerLevel'.  If 'level' is negative, suppress
    /// indentation of the first line.  If 'spacesPerLevel' is negative,
    /// suppress line breaks and format the entire output on one line.  If
    /// 'stream' is initially invalid, this operation has no effect.  Note that
    /// a trailing newline is provided in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// This type's default constructor is equivalent to setting each byte of
    /// the object's footprint to zero.
    NTSCFG_TYPE_TRAIT_BITWISE_INITIALIZABLE(CircularUint16);

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(CircularUint16);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(CircularUint16);

    /// This type's equality-comparison operator is equivalent to comparing
    /// each byte of one comparand's footprint to each corresponding byte of
    /// the other comparand's footprint. Note that this trait implies that an
    /// object of this type has no padding bytes between data members.
    NTSCFG_TYPE_TRAIT_BITWISE_COMPARABLE(CircularUint16);

  private:
    /// The value.
    bsl::uint16_t d_value;
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntsa::CircularUint16
bsl::ostream& operator<<(bsl::ostream& stream, const CircularUint16& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntsa::CircularUint16
bool operator==(const CircularUint16& lhs, const CircularUint16& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntsa::CircularUint16
bool operator!=(const CircularUint16& lhs, const CircularUint16& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsa::CircularUint16
bool operator<(const CircularUint16& lhs, const CircularUint16& rhs);

/// Return true if the value of the specified 'lhs' is less than or equal to
/// the value of the specified 'rhs', otherwise return false.
///
/// @related ntsa::CircularUint16
bool operator<=(const CircularUint16& lhs, const CircularUint16& rhs);

/// Return true if the value of the specified 'lhs' is greater than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsa::CircularUint16
bool operator>(const CircularUint16& lhs, const CircularUint16& rhs);

/// Return true if the value of the specified 'lhs' is greater than or equal to
/// the value of the specified 'rhs', otherwise return false.
///
/// @related ntsa::CircularUint16
bool operator>=(const CircularUint16& lhs, const CircularUint16& rhs);

/// Add the specified 'rhs' to the specified 'lhs' and return the result.
///
/// @related ntsa::CircularUint16
bsl::uint16_t operator+(const CircularUint16& lhs, const CircularUint16& rhs);

/// Add the specified 'rhs' to the specified 'lhs' and return the result.
///
/// @related ntsa::CircularUint16
CircularUint16 operator+(const CircularUint16& lhs, bsl::int16_t rhs);

/// Add the specified 'rhs' to the specified 'lhs' and return the result.
///
/// @related ntsa::CircularUint16
CircularUint16 operator+(const CircularUint16& lhs, bsl::uint16_t rhs);

/// Subtract the specified 'rhs' from the specified 'lhs' and return the
/// result.
///
/// @related ntsa::CircularUint16
bsl::int16_t operator-(const CircularUint16& lhs, const CircularUint16& rhs);

/// Subtract the specified 'rhs' from the specified 'lhs' and return the
/// result.
///
/// @related ntsa::CircularUint16
CircularUint16 operator-(const CircularUint16& lhs, bsl::int16_t rhs);

/// Subtract the specified 'rhs' from the specified 'lhs' and return the
/// result.
///
/// @related ntsa::CircularUint16
CircularUint16 operator-(const CircularUint16& lhs, bsl::uint16_t rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::CircularUint16
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const CircularUint16& value);

/// Describes 32-bit unsigned circular integer.
///
/// @details
/// Like fundamental two's complement unsigned integers, a circular integer is
/// permitted to "wrap around" when it reaches its minumum or maximum value.
/// But unlike fundamental two's complement unsigned integers, a circular
/// integer still maintains a valid definition of comparison and arithmetic
/// operators even after wrap-around occurs, provided one integer has
/// wrapped-around only one more time than the other.
///
/// These semantics are commonly required in sequence numbers used in
/// communications protocols.
///
/// For any two values stored there are two possible gaps between them. For
/// example, if one value is '5' and the other value is 'UINT_MAX - 10' then
/// the gap is either '15' or 'UINT_MAX - 15'.  All comparisons and arithmetic
/// performed by this type assume that the correct gap is the smaller of the
/// two possible gaps.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_data
class CircularUint32
{
  public:
    /// Defines a type alias for the value type.
    typedef bsl::uint32_t value_type;

    /// Defines a type alias for the difference type.
    typedef bsl::int32_t difference_type;

    /// Create a new 32-bit circular unsigned integer having the default value.
    CircularUint32();

    /// Create a new 32-bit circular unsigned integer having the specified
    /// 'value'.
    explicit CircularUint32(bsl::uint32_t value);

    /// Create a new 32-bit circular unsigned integer having the same value as
    /// the specified 'original' object. Assign an unspecified but valid value
    /// to the 'original' original.
    CircularUint32(bslmf::MovableRef<CircularUint32> original) NTSCFG_NOEXCEPT;

    /// Create a new 32-bit circular unsigned integer having the same value as
    /// the specifid 'original' value.
    CircularUint32(const CircularUint32& original);

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    CircularUint32& operator=(bslmf::MovableRef<CircularUint32> other)
        NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    CircularUint32& operator=(const CircularUint32& other);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    CircularUint32& operator=(bsl::uint32_t value);

    /// Increment the value of this object by one and return a reference to
    /// this modifiable object.
    CircularUint32& operator++();

    /// Increment the value of this object by one and return a copy of this
    /// object having the value of this object before it was incremented.
    CircularUint32 operator++(int);

    /// Decrement the value of this object by one and return a reference to
    /// this modifiable object.
    CircularUint32& operator--();

    /// Decrement the value of this object by one and return a copy of this
    /// object having the value of this object before it was decremented.
    CircularUint32 operator--(int);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Assign the specified 'value' to this object.
    void assign(bsl::uint32_t value);

    /// Increment the value by the specified 'delta'.
    void increment(bsl::uint32_t delta);

    /// Decrement the value by the specified 'delta'.
    void decrement(bsl::uint32_t delta);

    /// Return the value.
    bsl::uint32_t value() const;

    /// Return the signed difference between this object and the specified
    /// 'other' object. Note that if this object is greater than the 'target',
    /// the result is positive, if this object is less than the 'other' object
    /// the result is negative, and if the two objects have the same value the
    /// result is zero.
    bsl::int32_t difference(const CircularUint32& other) const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const CircularUint32& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const CircularUint32& other) const;

    /// Return true if the value of this object is less than or equal to the
    /// value of the specified 'other' object, otherwise return false.
    bool lessOrEquals(const CircularUint32& other) const;

    /// Contribute the values of the salient attributes of this object to the
    /// specified hash 'algorithm'.
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm) const;

    /// Format this object to the specified output 'stream' at the optionally
    /// specified indentation 'level' and return a reference to the modifiable
    /// 'stream'.  If 'level' is specified, optionally specify
    /// 'spacesPerLevel', the number of spaces per indentation level for this
    /// and all of its nested objects.  Each line is indented by the absolute
    /// value of 'level * spacesPerLevel'.  If 'level' is negative, suppress
    /// indentation of the first line.  If 'spacesPerLevel' is negative,
    /// suppress line breaks and format the entire output on one line.  If
    /// 'stream' is initially invalid, this operation has no effect.  Note that
    /// a trailing newline is provided in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// This type's default constructor is equivalent to setting each byte of
    /// the object's footprint to zero.
    NTSCFG_TYPE_TRAIT_BITWISE_INITIALIZABLE(CircularUint32);

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(CircularUint32);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(CircularUint32);

    /// This type's equality-comparison operator is equivalent to comparing
    /// each byte of one comparand's footprint to each corresponding byte of
    /// the other comparand's footprint. Note that this trait implies that an
    /// object of this type has no padding bytes between data members.
    NTSCFG_TYPE_TRAIT_BITWISE_COMPARABLE(CircularUint32);

  private:
    /// The value.
    bsl::uint32_t d_value;
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntsa::CircularUint32
bsl::ostream& operator<<(bsl::ostream& stream, const CircularUint32& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntsa::CircularUint32
bool operator==(const CircularUint32& lhs, const CircularUint32& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntsa::CircularUint32
bool operator!=(const CircularUint32& lhs, const CircularUint32& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsa::CircularUint32
bool operator<(const CircularUint32& lhs, const CircularUint32& rhs);

/// Return true if the value of the specified 'lhs' is less than or equal to
/// the value of the specified 'rhs', otherwise return false.
///
/// @related ntsa::CircularUint32
bool operator<=(const CircularUint32& lhs, const CircularUint32& rhs);

/// Return true if the value of the specified 'lhs' is greater than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsa::CircularUint32
bool operator>(const CircularUint32& lhs, const CircularUint32& rhs);

/// Return true if the value of the specified 'lhs' is greater than or equal to
/// the value of the specified 'rhs', otherwise return false.
///
/// @related ntsa::CircularUint32
bool operator>=(const CircularUint32& lhs, const CircularUint32& rhs);

/// Add the specified 'rhs' to the specified 'lhs' and return the result.
///
/// @related ntsa::CircularUint32
bsl::uint32_t operator+(const CircularUint32& lhs, const CircularUint32& rhs);

/// Add the specified 'rhs' to the specified 'lhs' and return the result.
///
/// @related ntsa::CircularUint32
CircularUint32 operator+(const CircularUint32& lhs, bsl::int32_t rhs);

/// Add the specified 'rhs' to the specified 'lhs' and return the result.
///
/// @related ntsa::CircularUint32
CircularUint32 operator+(const CircularUint32& lhs, bsl::uint32_t rhs);

/// Subtract the specified 'rhs' from the specified 'lhs' and return the
/// result.
///
/// @related ntsa::CircularUint32
bsl::int32_t operator-(const CircularUint32& lhs, const CircularUint32& rhs);

/// Subtract the specified 'rhs' from the specified 'lhs' and return the
/// result.
///
/// @related ntsa::CircularUint32
CircularUint32 operator-(const CircularUint32& lhs, bsl::int32_t rhs);

/// Subtract the specified 'rhs' from the specified 'lhs' and return the
/// result.
///
/// @related ntsa::CircularUint32
CircularUint32 operator-(const CircularUint32& lhs, bsl::uint32_t rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::CircularUint32
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const CircularUint32& value);

/// Describes 64-bit unsigned circular integer.
///
/// @details
/// Like fundamental two's complement unsigned integers, a circular integer is
/// permitted to "wrap around" when it reaches its minumum or maximum value.
/// But unlike fundamental two's complement unsigned integers, a circular
/// integer still maintains a valid definition of comparison and arithmetic
/// operators even after wrap-around occurs, provided one integer has
/// wrapped-around only one more time than the other.
///
/// These semantics are commonly required in sequence numbers used in
/// communications protocols.
///
/// For any two values stored there are two possible gaps between them. For
/// example, if one value is '5' and the other value is 'ULONG_LONG_MAX - 10'
/// then the gap is either '15' or 'ULONG_LONG_MAX - 15'.  All comparisons and
/// arithmetic performed by this type assume that the correct gap is the
/// smaller of the two possible gaps.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_data
class CircularUint64
{
  public:
    /// Defines a type alias for the value type.
    typedef bsl::uint64_t value_type;

    /// Defines a type alias for the difference type.
    typedef bsl::int64_t difference_type;

    /// Create a new 64-bit circular unsigned integer having the default value.
    CircularUint64();

    /// Create a new 64-bit circular unsigned integer having the specified
    /// 'value'.
    explicit CircularUint64(bsl::uint64_t value);

    /// Create a new 64-bit circular unsigned integer having the same value as
    /// the specified 'original' object. Assign an unspecified but valid value
    /// to the 'original' original.
    CircularUint64(bslmf::MovableRef<CircularUint64> original) NTSCFG_NOEXCEPT;

    /// Create a new 64-bit circular unsigned integer having the same value as
    /// the specifid 'original' value.
    CircularUint64(const CircularUint64& original);

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    CircularUint64& operator=(bslmf::MovableRef<CircularUint64> other)
        NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    CircularUint64& operator=(const CircularUint64& other);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    CircularUint64& operator=(bsl::uint64_t value);

    /// Increment the value of this object by one and return a reference to
    /// this modifiable object.
    CircularUint64& operator++();

    /// Increment the value of this object by one and return a copy of this
    /// object having the value of this object before it was incremented.
    CircularUint64 operator++(int);

    /// Decrement the value of this object by one and return a reference to
    /// this modifiable object.
    CircularUint64& operator--();

    /// Decrement the value of this object by one and return a copy of this
    /// object having the value of this object before it was decremented.
    CircularUint64 operator--(int);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Assign the specified 'value' to this object.
    void assign(bsl::uint64_t value);

    /// Increment the value by the specified 'delta'.
    void increment(bsl::uint64_t delta);

    /// Decrement the value by the specified 'delta'.
    void decrement(bsl::uint64_t delta);

    /// Return the value.
    bsl::uint64_t value() const;

    /// Return the signed difference between this object and the specified
    /// 'other' object. Note that if this object is greater than the 'target',
    /// the result is positive, if this object is less than the 'other' object
    /// the result is negative, and if the two objects have the same value the
    /// result is zero.
    bsl::int64_t difference(const CircularUint64& other) const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const CircularUint64& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const CircularUint64& other) const;

    /// Return true if the value of this object is less than or equal to the
    /// value of the specified 'other' object, otherwise return false.
    bool lessOrEquals(const CircularUint64& other) const;

    /// Contribute the values of the salient attributes of this object to the
    /// specified hash 'algorithm'.
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm) const;

    /// Format this object to the specified output 'stream' at the optionally
    /// specified indentation 'level' and return a reference to the modifiable
    /// 'stream'.  If 'level' is specified, optionally specify
    /// 'spacesPerLevel', the number of spaces per indentation level for this
    /// and all of its nested objects.  Each line is indented by the absolute
    /// value of 'level * spacesPerLevel'.  If 'level' is negative, suppress
    /// indentation of the first line.  If 'spacesPerLevel' is negative,
    /// suppress line breaks and format the entire output on one line.  If
    /// 'stream' is initially invalid, this operation has no effect.  Note that
    /// a trailing newline is provided in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// This type's default constructor is equivalent to setting each byte of
    /// the object's footprint to zero.
    NTSCFG_TYPE_TRAIT_BITWISE_INITIALIZABLE(CircularUint64);

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(CircularUint64);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(CircularUint64);

    /// This type's equality-comparison operator is equivalent to comparing
    /// each byte of one comparand's footprint to each corresponding byte of
    /// the other comparand's footprint. Note that this trait implies that an
    /// object of this type has no padding bytes between data members.
    NTSCFG_TYPE_TRAIT_BITWISE_COMPARABLE(CircularUint64);

  private:
    /// The value.
    bsl::uint64_t d_value;
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntsa::CircularUint64
bsl::ostream& operator<<(bsl::ostream& stream, const CircularUint64& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntsa::CircularUint64
bool operator==(const CircularUint64& lhs, const CircularUint64& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntsa::CircularUint64
bool operator!=(const CircularUint64& lhs, const CircularUint64& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsa::CircularUint64
bool operator<(const CircularUint64& lhs, const CircularUint64& rhs);

/// Return true if the value of the specified 'lhs' is less than or equal to
/// the value of the specified 'rhs', otherwise return false.
///
/// @related ntsa::CircularUint64
bool operator<=(const CircularUint64& lhs, const CircularUint64& rhs);

/// Return true if the value of the specified 'lhs' is greater than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsa::CircularUint64
bool operator>(const CircularUint64& lhs, const CircularUint64& rhs);

/// Return true if the value of the specified 'lhs' is greater than or equal to
/// the value of the specified 'rhs', otherwise return false.
///
/// @related ntsa::CircularUint64
bool operator>=(const CircularUint64& lhs, const CircularUint64& rhs);

/// Add the specified 'rhs' to the specified 'lhs' and return the result.
///
/// @related ntsa::CircularUint64
bsl::uint64_t operator+(const CircularUint64& lhs, const CircularUint64& rhs);

/// Add the specified 'rhs' to the specified 'lhs' and return the result.
///
/// @related ntsa::CircularUint64
CircularUint64 operator+(const CircularUint64& lhs, bsl::int64_t rhs);

/// Add the specified 'rhs' to the specified 'lhs' and return the result.
///
/// @related ntsa::CircularUint64
CircularUint64 operator+(const CircularUint64& lhs, bsl::uint64_t rhs);

/// Subtract the specified 'rhs' from the specified 'lhs' and return the
/// result.
///
/// @related ntsa::CircularUint64
bsl::int64_t operator-(const CircularUint64& lhs, const CircularUint64& rhs);

/// Subtract the specified 'rhs' from the specified 'lhs' and return the
/// result.
///
/// @related ntsa::CircularUint64
CircularUint64 operator-(const CircularUint64& lhs, bsl::int64_t rhs);

/// Subtract the specified 'rhs' from the specified 'lhs' and return the
/// result.
///
/// @related ntsa::CircularUint64
CircularUint64 operator-(const CircularUint64& lhs, bsl::uint64_t rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::CircularUint64
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const CircularUint64& value);

NTSCFG_INLINE
CircularUint16::CircularUint16()
: d_value(0)
{
    BSLMF_ASSERT(sizeof *this == sizeof(bsl::uint16_t));
}

NTSCFG_INLINE
CircularUint16::CircularUint16(bsl::uint16_t value)
: d_value(value)
{
    BSLMF_ASSERT(sizeof *this == sizeof(bsl::uint16_t));
}

NTSCFG_INLINE
CircularUint16::CircularUint16(bslmf::MovableRef<CircularUint16> original)
    NTSCFG_NOEXCEPT : d_value(NTSCFG_MOVE_FROM(original, d_value))
{
    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
CircularUint16::CircularUint16(const CircularUint16& original)
: d_value(original.d_value)
{
    BSLMF_ASSERT(sizeof *this == sizeof(bsl::uint16_t));
}

NTSCFG_INLINE
CircularUint16& CircularUint16::operator=(
    bslmf::MovableRef<CircularUint16> other) NTSCFG_NOEXCEPT
{
    d_value = NTSCFG_MOVE_FROM(other, d_value);
    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
CircularUint16& CircularUint16::operator=(const CircularUint16& other)
{
    d_value = other.d_value;
    return *this;
}

NTSCFG_INLINE
CircularUint16& CircularUint16::operator=(bsl::uint16_t value)
{
    d_value = value;
    return *this;
}

NTSCFG_INLINE
CircularUint16& CircularUint16::operator++()
{
    ++d_value;
    return *this;
}

NTSCFG_INLINE
CircularUint16 CircularUint16::operator++(int)
{
    CircularUint16 previous = *this;

    ++d_value;

    return previous;
}

NTSCFG_INLINE
CircularUint16& CircularUint16::operator--()
{
    --d_value;
    return *this;
}

NTSCFG_INLINE
CircularUint16 CircularUint16::operator--(int)
{
    CircularUint16 previous = *this;

    --d_value;

    return previous;
}

NTSCFG_INLINE
void CircularUint16::reset()
{
    d_value = 0;
}

NTSCFG_INLINE
void CircularUint16::assign(bsl::uint16_t value)
{
    d_value = value;
}

NTSCFG_INLINE
void CircularUint16::increment(bsl::uint16_t delta)
{
    d_value += delta;
}

NTSCFG_INLINE
void CircularUint16::decrement(bsl::uint16_t delta)
{
    d_value -= delta;
}

NTSCFG_INLINE
bsl::uint16_t CircularUint16::value() const
{
    return d_value;
}

NTSCFG_INLINE
bsl::int16_t CircularUint16::difference(const CircularUint16& other) const
{
    return (*this > other ? d_value - other.d_value
                          : -(other.d_value - d_value));
}

NTSCFG_INLINE
bool CircularUint16::equals(const CircularUint16& other) const
{
    return d_value == other.d_value;
}

NTSCFG_INLINE
bool CircularUint16::less(const CircularUint16& other) const
{
    const bsl::uint16_t threshold =
        bsl::numeric_limits<bsl::uint16_t>::max() / 2;

    const bsl::uint16_t difference =
        static_cast<bsl::uint16_t>(other.d_value - d_value);

    return ((difference != 0) && (difference < threshold));
}

NTSCFG_INLINE
bool CircularUint16::lessOrEquals(const CircularUint16& other) const
{
    const bsl::uint16_t threshold =
        bsl::numeric_limits<bsl::uint16_t>::max() / 2;

    const bsl::uint16_t difference =
        static_cast<bsl::uint16_t>(other.d_value - d_value);

    return difference < threshold;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void CircularUint16::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    hashAppend(algorithm, d_value);
}

NTSCFG_INLINE
bsl::ostream& CircularUint16::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    NTSCFG_WARNING_UNUSED(level);
    NTSCFG_WARNING_UNUSED(spacesPerLevel);

    return stream << d_value;
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const CircularUint16& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const CircularUint16& lhs, const CircularUint16& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const CircularUint16& lhs, const CircularUint16& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const CircularUint16& lhs, const CircularUint16& rhs)
{
    return lhs.less(rhs);
}

NTSCFG_INLINE
bool operator<=(const CircularUint16& lhs, const CircularUint16& rhs)
{
    return lhs.lessOrEquals(rhs);
}

NTSCFG_INLINE bool operator>(const CircularUint16& lhs,
                             const CircularUint16& rhs)
{
    return !(lhs <= rhs);
}

NTSCFG_INLINE bool operator>=(const CircularUint16& lhs,
                              const CircularUint16& rhs)
{
    return !(lhs < rhs);
}

NTSCFG_INLINE
bsl::uint16_t operator+(const CircularUint16& lhs, const CircularUint16& rhs)
{
    return lhs.value() + rhs.value();
}

NTSCFG_INLINE
CircularUint16 operator+(const CircularUint16& lhs, bsl::int16_t rhs)
{
    return CircularUint16(lhs.value() + rhs);
}

NTSCFG_INLINE
CircularUint16 operator+(const CircularUint16& lhs, bsl::uint16_t rhs)
{
    return CircularUint16(lhs.value() + rhs);
}

NTSCFG_INLINE
bsl::int16_t operator-(const CircularUint16& lhs, const CircularUint16& rhs)
{
    return lhs.difference(rhs);
}

NTSCFG_INLINE
CircularUint16 operator-(const CircularUint16& lhs, bsl::int16_t rhs)
{
    return CircularUint16(lhs.value() - rhs);
}

NTSCFG_INLINE
CircularUint16 operator-(const CircularUint16& lhs, bsl::uint16_t rhs)
{
    return CircularUint16(lhs.value() - rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&       algorithm,
                              const CircularUint16& value)
{
    value.hash(algorithm);
}

NTSCFG_INLINE
CircularUint32::CircularUint32()
: d_value(0)
{
    BSLMF_ASSERT(sizeof *this == sizeof(bsl::uint32_t));
}

NTSCFG_INLINE
CircularUint32::CircularUint32(bsl::uint32_t value)
: d_value(value)
{
    BSLMF_ASSERT(sizeof *this == sizeof(bsl::uint32_t));
}

NTSCFG_INLINE
CircularUint32::CircularUint32(bslmf::MovableRef<CircularUint32> original)
    NTSCFG_NOEXCEPT : d_value(NTSCFG_MOVE_FROM(original, d_value))
{
    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
CircularUint32::CircularUint32(const CircularUint32& original)
: d_value(original.d_value)
{
    BSLMF_ASSERT(sizeof *this == sizeof(bsl::uint32_t));
}

NTSCFG_INLINE
CircularUint32& CircularUint32::operator=(
    bslmf::MovableRef<CircularUint32> other) NTSCFG_NOEXCEPT
{
    d_value = NTSCFG_MOVE_FROM(other, d_value);
    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
CircularUint32& CircularUint32::operator=(const CircularUint32& other)
{
    d_value = other.d_value;
    return *this;
}

NTSCFG_INLINE
CircularUint32& CircularUint32::operator=(bsl::uint32_t value)
{
    d_value = value;
    return *this;
}

NTSCFG_INLINE
CircularUint32& CircularUint32::operator++()
{
    ++d_value;
    return *this;
}

NTSCFG_INLINE
CircularUint32 CircularUint32::operator++(int)
{
    CircularUint32 previous = *this;

    ++d_value;

    return previous;
}

NTSCFG_INLINE
CircularUint32& CircularUint32::operator--()
{
    --d_value;
    return *this;
}

NTSCFG_INLINE
CircularUint32 CircularUint32::operator--(int)
{
    CircularUint32 previous = *this;

    --d_value;

    return previous;
}

NTSCFG_INLINE
void CircularUint32::reset()
{
    d_value = 0;
}

NTSCFG_INLINE
void CircularUint32::assign(bsl::uint32_t value)
{
    d_value = value;
}

NTSCFG_INLINE
void CircularUint32::increment(bsl::uint32_t delta)
{
    d_value += delta;
}

NTSCFG_INLINE
void CircularUint32::decrement(bsl::uint32_t delta)
{
    d_value -= delta;
}

NTSCFG_INLINE
bsl::uint32_t CircularUint32::value() const
{
    return d_value;
}

NTSCFG_INLINE
bsl::int32_t CircularUint32::difference(const CircularUint32& other) const
{
    return (*this > other ? d_value - other.d_value
                          : -(other.d_value - d_value));
}

NTSCFG_INLINE
bool CircularUint32::equals(const CircularUint32& other) const
{
    return d_value == other.d_value;
}

NTSCFG_INLINE
bool CircularUint32::less(const CircularUint32& other) const
{
    const bsl::uint32_t threshold =
        bsl::numeric_limits<bsl::uint32_t>::max() / 2;

    const bsl::uint32_t difference =
        static_cast<bsl::uint32_t>(other.d_value - d_value);

    return ((difference != 0) && (difference < threshold));
}

NTSCFG_INLINE
bool CircularUint32::lessOrEquals(const CircularUint32& other) const
{
    const bsl::uint32_t threshold =
        bsl::numeric_limits<bsl::uint32_t>::max() / 2;

    const bsl::uint32_t difference =
        static_cast<bsl::uint32_t>(other.d_value - d_value);

    return difference < threshold;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void CircularUint32::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    hashAppend(algorithm, d_value);
}

NTSCFG_INLINE
bsl::ostream& CircularUint32::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    NTSCFG_WARNING_UNUSED(level);
    NTSCFG_WARNING_UNUSED(spacesPerLevel);

    return stream << d_value;
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const CircularUint32& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const CircularUint32& lhs, const CircularUint32& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const CircularUint32& lhs, const CircularUint32& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const CircularUint32& lhs, const CircularUint32& rhs)
{
    return lhs.less(rhs);
}

NTSCFG_INLINE
bool operator<=(const CircularUint32& lhs, const CircularUint32& rhs)
{
    return lhs.lessOrEquals(rhs);
}

NTSCFG_INLINE bool operator>(const CircularUint32& lhs,
                             const CircularUint32& rhs)
{
    return !(lhs <= rhs);
}

NTSCFG_INLINE bool operator>=(const CircularUint32& lhs,
                              const CircularUint32& rhs)
{
    return !(lhs < rhs);
}

NTSCFG_INLINE
bsl::uint32_t operator+(const CircularUint32& lhs, const CircularUint32& rhs)
{
    return lhs.value() + rhs.value();
}

NTSCFG_INLINE
CircularUint32 operator+(const CircularUint32& lhs, bsl::int32_t rhs)
{
    return CircularUint32(lhs.value() + rhs);
}

NTSCFG_INLINE
CircularUint32 operator+(const CircularUint32& lhs, bsl::uint32_t rhs)
{
    return CircularUint32(lhs.value() + rhs);
}

NTSCFG_INLINE
bsl::int32_t operator-(const CircularUint32& lhs, const CircularUint32& rhs)
{
    return lhs.difference(rhs);
}

NTSCFG_INLINE
CircularUint32 operator-(const CircularUint32& lhs, bsl::int32_t rhs)
{
    return CircularUint32(lhs.value() - rhs);
}

NTSCFG_INLINE
CircularUint32 operator-(const CircularUint32& lhs, bsl::uint32_t rhs)
{
    return CircularUint32(lhs.value() - rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&       algorithm,
                              const CircularUint32& value)
{
    value.hash(algorithm);
}

NTSCFG_INLINE
CircularUint64::CircularUint64()
: d_value(0)
{
    BSLMF_ASSERT(sizeof *this == sizeof(bsl::uint64_t));
}

NTSCFG_INLINE
CircularUint64::CircularUint64(bsl::uint64_t value)
: d_value(value)
{
    BSLMF_ASSERT(sizeof *this == sizeof(bsl::uint64_t));
}

NTSCFG_INLINE
CircularUint64::CircularUint64(bslmf::MovableRef<CircularUint64> original)
    NTSCFG_NOEXCEPT : d_value(NTSCFG_MOVE_FROM(original, d_value))
{
    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
CircularUint64::CircularUint64(const CircularUint64& original)
: d_value(original.d_value)
{
    BSLMF_ASSERT(sizeof *this == sizeof(bsl::uint64_t));
}

NTSCFG_INLINE
CircularUint64& CircularUint64::operator=(
    bslmf::MovableRef<CircularUint64> other) NTSCFG_NOEXCEPT
{
    d_value = NTSCFG_MOVE_FROM(other, d_value);
    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
CircularUint64& CircularUint64::operator=(const CircularUint64& other)
{
    d_value = other.d_value;
    return *this;
}

NTSCFG_INLINE
CircularUint64& CircularUint64::operator=(bsl::uint64_t value)
{
    d_value = value;
    return *this;
}

NTSCFG_INLINE
CircularUint64& CircularUint64::operator++()
{
    ++d_value;
    return *this;
}

NTSCFG_INLINE
CircularUint64 CircularUint64::operator++(int)
{
    CircularUint64 previous = *this;

    ++d_value;

    return previous;
}

NTSCFG_INLINE
CircularUint64& CircularUint64::operator--()
{
    --d_value;
    return *this;
}

NTSCFG_INLINE
CircularUint64 CircularUint64::operator--(int)
{
    CircularUint64 previous = *this;

    --d_value;

    return previous;
}

NTSCFG_INLINE
void CircularUint64::reset()
{
    d_value = 0;
}

NTSCFG_INLINE
void CircularUint64::assign(bsl::uint64_t value)
{
    d_value = value;
}

NTSCFG_INLINE
void CircularUint64::increment(bsl::uint64_t delta)
{
    d_value += delta;
}

NTSCFG_INLINE
void CircularUint64::decrement(bsl::uint64_t delta)
{
    d_value -= delta;
}

NTSCFG_INLINE
bsl::uint64_t CircularUint64::value() const
{
    return d_value;
}

NTSCFG_INLINE
bsl::int64_t CircularUint64::difference(const CircularUint64& other) const
{
    return (*this > other ? d_value - other.d_value
                          : -(other.d_value - d_value));
}

NTSCFG_INLINE
bool CircularUint64::equals(const CircularUint64& other) const
{
    return d_value == other.d_value;
}

NTSCFG_INLINE
bool CircularUint64::less(const CircularUint64& other) const
{
    const bsl::uint64_t threshold =
        bsl::numeric_limits<bsl::uint64_t>::max() / 2;

    const bsl::uint64_t difference =
        static_cast<bsl::uint64_t>(other.d_value - d_value);

    return ((difference != 0) && (difference < threshold));
}

NTSCFG_INLINE
bool CircularUint64::lessOrEquals(const CircularUint64& other) const
{
    const bsl::uint64_t threshold =
        bsl::numeric_limits<bsl::uint64_t>::max() / 2;

    const bsl::uint64_t difference =
        static_cast<bsl::uint64_t>(other.d_value - d_value);

    return difference < threshold;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void CircularUint64::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    hashAppend(algorithm, d_value);
}

NTSCFG_INLINE
bsl::ostream& CircularUint64::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    NTSCFG_WARNING_UNUSED(level);
    NTSCFG_WARNING_UNUSED(spacesPerLevel);

    return stream << d_value;
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const CircularUint64& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const CircularUint64& lhs, const CircularUint64& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const CircularUint64& lhs, const CircularUint64& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const CircularUint64& lhs, const CircularUint64& rhs)
{
    return lhs.less(rhs);
}

NTSCFG_INLINE
bool operator<=(const CircularUint64& lhs, const CircularUint64& rhs)
{
    return lhs.lessOrEquals(rhs);
}

NTSCFG_INLINE bool operator>(const CircularUint64& lhs,
                             const CircularUint64& rhs)
{
    return !(lhs <= rhs);
}

NTSCFG_INLINE bool operator>=(const CircularUint64& lhs,
                              const CircularUint64& rhs)
{
    return !(lhs < rhs);
}

NTSCFG_INLINE
bsl::uint64_t operator+(const CircularUint64& lhs, const CircularUint64& rhs)
{
    return lhs.value() + rhs.value();
}

NTSCFG_INLINE
CircularUint64 operator+(const CircularUint64& lhs, bsl::int64_t rhs)
{
    return CircularUint64(lhs.value() + rhs);
}

NTSCFG_INLINE
CircularUint64 operator+(const CircularUint64& lhs, bsl::uint64_t rhs)
{
    return CircularUint64(lhs.value() + rhs);
}

NTSCFG_INLINE
bsl::int64_t operator-(const CircularUint64& lhs, const CircularUint64& rhs)
{
    return lhs.difference(rhs);
}

NTSCFG_INLINE
CircularUint64 operator-(const CircularUint64& lhs, bsl::int64_t rhs)
{
    return CircularUint64(lhs.value() - rhs);
}

NTSCFG_INLINE
CircularUint64 operator-(const CircularUint64& lhs, bsl::uint64_t rhs)
{
    return CircularUint64(lhs.value() - rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&       algorithm,
                              const CircularUint64& value)
{
    value.hash(algorithm);
}

}  // close namespace ntsa
}  // close namespace BloombergLP
#endif
