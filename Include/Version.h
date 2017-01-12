//
/// @file Include/Version.h
///
/// Project versioning
///

#pragma once
#ifndef __PROJECT_VERSION_HEADER__
#define __PROJECT_VERSION_HEADER__

#include <ProjectVersion.h>

#include <Uefi.h>

// PROJECT_NAME
/// The project name string
#define PROJECT_NAME _PROJECT_NAME
// PROJECT_NAME_A
/// The project name string in ASCII
#define PROJECT_NAME_A _PROJECT_NAME_A
// PROJECT_SAFE_NAME
/// The project name string, safe for filesystems
#define PROJECT_SAFE_NAME _PROJECT_SAFE_NAME
// PROJECT_SAFE_NAME_A
/// The project name string in ASCII, safe for filesystems
#define PROJECT_SAFE_NAME_A _PROJECT_SAFE_NAME_A
// PROJECT_VERSION
/// The project version string
#define PROJECT_VERSION _PROJECT_VERSION
// PROJECT_VERSION_A
/// The project version string in ASCII
#define PROJECT_VERSION_A _PROJECT_VERSION_A
// PROJECT_VERSIONCODE
/// The project version code
#define PROJECT_VERSIONCODE _PROJECT_VERSIONCODE
// PROJECT_DATETIME
/// The project build date and time string
#define PROJECT_DATETIME _PROJECT_DATETIME
// PROJECT_DATETIME_A
/// The project build date and time string in ASCII
#define PROJECT_DATETIME_A _PROJECT_DATETIME_A
// PROJECT_FIRMWARE_VERSION
/// The firmware UEFI specification compliance version string
#define PROJECT_FIRMWARE_VERSION _PROJECT_FIRMWARE_VERSION
// PROJECT_FIRMWARE_VERSION_A
/// The firmware UEFI specification compliance version string in ASCII
#define PROJECT_FIRMWARE_VERSION_A _PROJECT_FIRMWARE_VERSION_A
// PROJECT_FIRMWARE_VERSIONCODE
/// The firmware UEFI specification compliance version code
#define PROJECT_FIRMWARE_VERSIONCODE _PROJECT_FIRMWARE_VERSIONCODE

// VERSIONCODE
/// A packed integer representation of a major.minor.update version.<br>
/// The update version is packed in bits 0 ... 11<br>
/// The minor version is packed in bits 12 ... 23<br>
/// The major version is packed in bits 24 ... 31<br>
typedef UINT32 VERSIONCODE;

// SIGNATURE
/// A packed represenation of a four character signature
typedef UINT32 SIGNATURE;

// PACK_SIGNATURE
/// Pack a signature
#define PACK_SIGNATURE(s1, s2, s3, s4) \
  (SIGNATURE)((((SIGNATURE)s1) & 0xFF) | ((((SIGNATURE)s2) << 8) & 0xFF00) | \
              ((((SIGNATURE)s3) << 16) & 0xFF0000) | ((((SIGNATURE)s4) << 24) & 0xFF000000))

// PACK_VERSIONCODE_FULL
/// Pack a full version code
/// @param major  The major version
/// @param minor  The minor version
/// @param update The update version
/// @return The version code
#define PACK_VERSIONCODE_FULL(major, minor, update) \
  (VERSIONCODE)((((VERSIONCODE)(major) & 0xff) << 24) | (((VERSIONCODE)(minor) & 0xfff) << 12) | ((VERSIONCODE)(update) & 0xfff))

// PACK_VERSIONCODE
/// Pack a version code
/// @param major  The major version
/// @param minor  The minor version
/// @return The version code
#define PACK_VERSIONCODE(major, minor) PACK_VERSIONCODE_FULL((UINTN)(major), (UINTN)(minor), (UINTN)(0))

// PACK_VERSIONCODE_SHORT
/// Pack a short version code
/// @param major  The major version
/// @return The version code
#define PACK_VERSIONCODE_SHORT(major) PACK_VERSIONCODE((UINTN)(major), (UINTN)(0))

// UNPACK_VERSIONCODE_MAJOR
/// Unpack the major version from the version code
/// @param version The version code to unpack
/// @return The major version
#define UNPACK_VERSIONCODE_MAJOR(version) (((VERSIONCODE)(version) >> 24) & 0xff)

// UNPACK_VERSIONCODE_MINOR
/// Unpack the minor version from the version code
/// @param version The version code to unpack
/// @return The minor version
#define UNPACK_VERSIONCODE_MINOR(version) (((VERSIONCODE)(version) >> 12) & 0xfff)

// UNPACK_VERSIONCODE_UPDATE
/// Unpack the update version from the version code
/// @param version The version code to unpack
/// @return The update version
#define UNPACK_VERSIONCODE_UPDATE(version) ((VERSIONCODE)(version) & 0xfff)

// ARRAY_COUNT
/// Get the count of elements in an array
/// @param arr The array to get the count of elements
/// @return The count of elements in the array
#define ARRAY_COUNT(arr) ((arr == NULL) ? 0 : ((sizeof(arr) <= sizeof(arr[0])) ? 1 : (sizeof(arr) / sizeof(arr[0]))))

// ADDRESS_OF
/// Get the address of a structure member
/// @param INSTANCETYPE The type of the instance structure
/// @param Instance     An instance of a structure to get the address of a member
/// @param FIELDTYPE    The type of the member field
/// @param Field        The name of the field of which to get the address
/// @return The address of the offset of the member field in the instance structure
#define ADDRESS_OF(INSTANCETYPE, Instance, FIELDTYPE, Field) (FIELDTYPE *)(((UINT8 *)(Instance)) + OFFSET_OF(INSTANCETYPE, Field))

// ADDRESS_OFFSET
/// Offsets an address by a specified number of bytes
/// @param ADDRESSTYPE The type of the returned address
/// @param Address     The address to offset
/// @param Offset      The offset in bytes
/// @return The offset address
#define ADDRESS_OFFSET(ADDRESSTYPE, Address, Offset) (ADDRESSTYPE *)(((UINT8 *)(Address)) + (Offset))

// ADDRESS_OFFSET_TYPE
/// Offsets an address by the size of another type
/// @param ADDRESSTYPE The type of the returned address
/// @param Address     The address to offset
/// @param OFFSETTYPE  The type of the offset
/// @return The offset address
#define ADDRESS_OFFSET_TYPE(ADDRESSTYPE, Address, OFFSETTYPE) ADDRESS_OFFSET(ADDRESSTYPE, Address, sizeof(OFFSETTYPE))

// ADDRESS_OFFSET_ANY
/// Offsets an address by a specified number of bytes
/// @param Address     The address to offset
/// @param Offset      The offset in bytes
/// @return The offset address
#define ADDRESS_OFFSET_ANY(Address, Offset) ADDRESS_OFFSET(VOID, Address, Offset)

// ADDRESS_OFFSET_ANY_TYPE
/// Offsets an address by the size of another type
/// @param Address     The address to offset
/// @param OFFSETTYPE  The type of the offset
/// @return The offset address
#define ADDRESS_OFFSET_ANY_TYPE(Address, OFFSETTYPE) ADDRESS_OFFSET_TYPE(VOID, Address, OFFSETTYPE)

#endif // __PROJECT_VERSION_HEADER__
