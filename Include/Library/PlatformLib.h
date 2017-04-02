//
/// @file Include/Library/PlatformLib.h
///
/// Platform information library
///

#pragma once
#ifndef __CPU_INFORMATION_LIBRARY_HEADER__
#define __CPU_INFORMATION_LIBRARY_HEADER__

#include <Library/LogLib.h>

#include <Protocol/PciIo.h>

// PACKAGE_COUNT
/// The maximum count of physical packages
#define PACKAGE_COUNT 4

// CPU_TURBO_COUNT
/// The maximum count of turbo multipliers
#define CPU_TURBO_COUNT 8
// CPU_DESCRIPTION_SIZE
/// The size of CPU description string in characters
#define CPU_DESCRIPTION_SIZE 48

// SLOT_COUNT
/// The maximum count of physical RAM slots
#define SLOT_COUNT 24

// FIND_DEVICE_VENDOR_ID
/// Find devices by vendor id
#define FIND_DEVICE_VENDOR_ID 0x1
// FIND_DEVICE_DEVICE_ID
/// Find devices by device id
#define FIND_DEVICE_DEVICE_ID 0x2
// FIND_DEVICE_BASE_CLASS
/// Find devices by base class
#define FIND_DEVICE_BASE_CLASS 0x4
// FIND_DEVICE_SUB_CLASS
/// Find devices by sub class
#define FIND_DEVICE_SUB_CLASS 0x8
// FIND_DEVICE_ALL
/// Find devices by vendor id, device id, base class and sub class
#define FIND_DEVICE_ALL (FIND_DEVICE_VENDOR_ID | FIND_DEVICE_DEVICE_ID | FIND_DEVICE_BASE_CLASS | FIND_DEVICE_SUB_CLASS)

// CPU_VENDOR
/// CPU vendor information
typedef enum _CPU_VENDOR CPU_VENDOR;
enum _CPU_VENDOR {

  // CPU_VENDOR_UNKNOWN
  /// Unknown CPU vendor
  CPU_VENDOR_UNKNOWN = 0,

  //
  // Physical CPU vendors
  //

  // CPU_VENDOR_INTEL
  /// Intel CPU vendor
  CPU_VENDOR_INTEL,
  // CPU_VENDOR_AMD
  /// AMD CPU vendor
  CPU_VENDOR_AMD,

  //
  // Virtual CPU vendors
  //

  // CPU_VENDOR_VIRTUAL_KVM
  /// KVM virtual machine
  CPU_VENDOR_VIRTUAL_KVM,
  // CPU_VENDOR_VIRTUAL_MSHVM
  /// Microsoft Hyper-V/Windows Virtual PC virtual machine
  CPU_VENDOR_VIRTUAL_MICROSOFT,
  // CPU_VENDOR_VIRTUAL_PARALLELS
  /// Parallels virtual machine
  CPU_VENDOR_VIRTUAL_PARALLELS,
  // CPU_VENDOR_VIRTUAL_VMWARE
  /// VMWare virtual machine
  CPU_VENDOR_VIRTUAL_VMWARE,
  // CPU_VENDOR_VIRTUAL_XENHVM
  /// Xen virtual machine
  CPU_VENDOR_VIRTUAL_XEN,

};

// CPU_FAMILY
/// CPU family information
typedef enum _CPU_FAMILY CPU_FAMILY;
enum _CPU_FAMILY {

  // CPU_FAMILY_UNKNOWN
  /// Unknown CPU family
  CPU_FAMILY_UNKNOWN = 0,

#if defined(MDE_CPU_IA32)

  // CPU_FAMILY_486
  /// 486 compatible CPU family
  CPU_FAMILY_486,

  // CPU_FAMILY_PENTIUM
  /// Intel Pentium compatible CPU family
  CPU_FAMILY_PENTIUM,
  // CPU_FAMILY_MOBILE_PENTIUM
  /// Intel Mobile Pentium compatible CPU family
  CPU_FAMILY_MOBILE_PENTIUM,
  // CPU_FAMILY_PENTIUM_OVERDRIVE
  /// Intel Pentium compatible CPU family
  CPU_FAMILY_PENTIUM_OVERDRIVE,

  // CPU_FAMILY_PENTIUM_MMX
  /// Intel Pentium MMX compatible CPU family
  CPU_FAMILY_PENTIUM_MMX,
  // CPU_FAMILY_MOBILE_PENTIUM_MMX
  /// Intel Mobile Pentium MMX compatible CPU family
  CPU_FAMILY_MOBILE_PENTIUM_MMX,
  // CPU_FAMILY_PENTIUM_MMX_OVERDRIVE
  /// Intel Pentium MMX compatible CPU family
  CPU_FAMILY_PENTIUM_MMX_OVERDRIVE,

  // CPU_FAMILY_PENTIUM_PRO
  /// Intel Pentium Pro compatible CPU family
  CPU_FAMILY_PENTIUM_PRO,

  // CPU_FAMILY_PENTIUM_II
  /// Intel Pentium II compatible CPU family
  CPU_FAMILY_PENTIUM_II,
  // CPU_FAMILY_MOBILE_PENTIUM_II
  /// Intel Mobile Pentium II compatible CPU family
  CPU_FAMILY_MOBILE_PENTIUM_II,
  // CPU_FAMILY_PENTIUM_II_OVERDRIVE
  /// Intel Pentium II Overdrive compatible CPU family
  CPU_FAMILY_PENTIUM_II_OVERDRIVE,

  // CPU_FAMILY_PENTIUM_III
  /// Intel Pentium III compatible CPU family
  CPU_FAMILY_PENTIUM_III,

  // CPU_FAMILY_XEON_PENTIUM_III
  /// Intel Pentium III Xeon compatible CPU family
  CPU_FAMILY_XEON_PENTIUM_III,

#endif

#if defined(MDE_CPU_IA32) || defined(MDE_CPU_X64)

  // CPU_FAMILY_PENTIUM_4
  /// Intel Pentium 4 compatible CPU family
  CPU_FAMILY_PENTIUM_4,

  // CPU_FAMILY_PENTIUM_D
  /// Intel Pentium D compatible CPU family
  CPU_FAMILY_PENTIUM_D,

  // CPU_FAMILY_PENTIUM_M
  /// Intel Pentium M compatible CPU family
  CPU_FAMILY_PENTIUM_M,

  // CPU_FAMILY_CELERON
  /// Intel Celeron compatible CPU family
  CPU_FAMILY_CELERON,

  // CPU_FAMILY_CELERON_D
  /// Intel Celeron D compatible CPU family
  CPU_FAMILY_CELERON_D,

  // CPU_FAMILY_CORE
  /// Intel Core compatible CPU family
  CPU_FAMILY_CORE,

  // CPU_FAMILY_CORE2
  /// Intel Core2 compatible CPU family
  CPU_FAMILY_CORE2,
  // CPU_FAMILY_CORE2_MOBILE
  /// Intel Core2 Mobile compatible CPU family
  CPU_FAMILY_CORE2_MOBILE,

  // CPU_FAMILY_CORE_i
  /// Intel Core i compatible CPU family
  CPU_FAMILY_CORE_i,
  // CPU_FAMILY_2ND_CORE_i
  /// Intel 2nd Gen Core i compatible CPU family
  CPU_FAMILY_2ND_CORE_i,
  // CPU_FAMILY_3RD_CORE_i
  /// Intel 3rd Gen Core i compatible CPU family
  CPU_FAMILY_3RD_CORE_i,
  // CPU_FAMILY_4TH_CORE_i
  /// Intel 4th Gen Core i compatible CPU family
  CPU_FAMILY_4TH_CORE_i,
  // CPU_FAMILY_5TH_CORE_i
  /// Intel 5th Gen Core i compatible CPU family
  CPU_FAMILY_5TH_CORE_i,
  // CPU_FAMILY_6TH_CORE_i
  /// Intel 6th Gen Core i compatible CPU family
  CPU_FAMILY_6TH_CORE_i,
  // CPU_FAMILY_7TH_CORE_i
  /// Intel 7th Gen Core i compatible CPU family
  CPU_FAMILY_7TH_CORE_i,

  // CPU_FAMILY_CORE_i_MOBILE
  /// Intel Core i Mobile compatible CPU family
  CPU_FAMILY_CORE_i_MOBILE,
  // CPU_FAMILY_2ND_CORE_i_MOBILE
  /// Intel2nd Gen Core i Mobile compatible CPU family
  CPU_FAMILY_2ND_CORE_i_MOBILE,
  // CPU_FAMILY_3RD_CORE_i_MOBILE
  /// Intel 3rd Gen Core i Mobile compatible CPU family
  CPU_FAMILY_3RD_CORE_i_MOBILE,
  // CPU_FAMILY_4TH_CORE_i_MOBILE
  /// Intel 4th Gen Core i Mobile compatible CPU family
  CPU_FAMILY_4TH_CORE_i_MOBILE,
  // CPU_FAMILY_5TH_CORE_i_MOBILE
  /// Intel 5th Gen Core i Mobile compatible CPU family
  CPU_FAMILY_5TH_CORE_i_MOBILE,
  // CPU_FAMILY_6TH_CORE_i_MOBILE
  /// Intel 6th Gen Core i Mobile compatible CPU family
  CPU_FAMILY_6TH_CORE_i_MOBILE,
  // CPU_FAMILY_7TH_CORE_i_MOBILE
  /// Intel 7th Gen Core i Mobile compatible CPU family
  CPU_FAMILY_7TH_CORE_i_MOBILE,

  // CPU_FAMILY_XEON
  /// Intel Xeon compatible CPU family
  CPU_FAMILY_XEON,
  // CPU_FAMILY_XEON_MP
  /// Intel Xeon MP compatible CPU family
  CPU_FAMILY_XEON_MP,
  // CPU_FAMILY_XEON_CORE2
  /// Intel Xeon Core2 compatible CPU family
  CPU_FAMILY_XEON_CORE2,
  // CPU_FAMILY_XEON_CORE_i
  /// Intel Xeon Core i compatible CPU family
  CPU_FAMILY_XEON_CORE_i,
  // CPU_FAMILY_XEON_2ND_CORE_i
  /// Intel Xeon 2nd Gen Core i compatible CPU family
  CPU_FAMILY_XEON_2ND_CORE_i,
  // CPU_FAMILY_XEON_3RD_CORE_i
  /// Intel Xeon 3rd Gen Core i compatible CPU family
  CPU_FAMILY_XEON_3RD_CORE_i,
  // CPU_FAMILY_XEON_4TH_CORE_i
  /// Intel Xeon 4th Gen Core i compatible CPU family
  CPU_FAMILY_XEON_4TH_CORE_i,
  // CPU_FAMILY_XEON_5TH_CORE_i
  /// Intel Xeon 5th Gen Core i compatible CPU family
  CPU_FAMILY_XEON_5TH_CORE_i,
  // CPU_FAMILY_XEON_6TH_CORE_i
  /// Intel Xeon 6th Gen Core i compatible CPU family
  CPU_FAMILY_XEON_6TH_CORE_i,
  // CPU_FAMILY_XEON_7TH_CORE_i
  /// Intel Xeon 7th Gen Core i compatible CPU family
  CPU_FAMILY_XEON_7TH_CORE_i,

  // CPU_FAMILY_XEON_PHI
  /// Intel Xeon Phi compatible CPU family
  CPU_FAMILY_XEON_PHI,

  // CPU_FAMILY_ATOM
  /// Intel Atom compatible CPU family
  CPU_FAMILY_ATOM,
  // CPU_FAMILY_2ND_ATOM
  /// Intel 2nd Gen Atom compatible CPU family
  CPU_FAMILY_2ND_ATOM,
  // CPU_FAMILY_3RD_ATOM
  /// Intel 3rd Gen Atom compatible CPU family
  CPU_FAMILY_3RD_ATOM,
  // CPU_FAMILY_4TH_ATOM
  /// Intel 4th Gen Atom compatible CPU family
  CPU_FAMILY_4TH_ATOM,

#endif

#if defined(MDE_CPU_IA32) || defined(MDE_CPU_IPF)

  // CPU_FAMILY_ITANIUM
  /// Intel Itanium compatible CPU family
  CPU_FAMILY_ITANIUM,
  // CPU_FAMILY_ITANIUM2
  /// Intel Itanium 2 compatible CPU family
  CPU_FAMILY_ITANIUM2,

#endif

};

// CPU_MODEL
/// CPU model information
typedef UINT32 CPU_MODEL;

// CPU_MODEL_UNKNOWN
/// Unknown CPU model
#define CPU_MODEL_UNKNOWN 0

// GetCPUVendorString
/// Get the CPU vendor string
/// @param Vendor The CPU vendor identifier
/// @return The CPU vendor string
CHAR16 *
GetCPUVendorString (
  CPU_VENDOR Vendor
);
// GetCPUFamilyString
/// Get the CPU family string
/// @param Family The CPU family identifier
/// @return The CPU family string
CHAR16 *
GetCPUFamilyString (
  CPU_FAMILY Family
);

// GetCPUVendor
/// Get CPU vendor information
/// @return The CPU vendor information
CPU_VENDOR
EFIAPI
GetCPUVendor (
  VOID
);
// GetCPUFamily
/// Get CPU family information
/// @return The CPU family information
CPU_FAMILY
EFIAPI
GetCPUFamily (
  VOID
);
// GetCPUModel
/// Get CPU model information
/// @return The CPU model information
CPU_MODEL
EFIAPI
GetCPUModel (
  VOID
);
// GetCPUFrequency
/// Get CPU frequency in Hz
/// @return The CPU frequency in Hz
UINT64
EFIAPI
GetCPUFrequency (
  VOID
);
// IsCPUMobile
/// Check if CPU is mobile
BOOLEAN
EFIAPI
IsCPUMobile (
  VOID
);
// IsCPUBigEndian
/// Check if CPU is little-endian byte order
BOOLEAN
EFIAPI
IsCPUBigEndian (
  VOID
);
// IsCPULittleEndian
/// Check if CPU is little-endian byte order
BOOLEAN
EFIAPI
IsCPULittleEndian (
  VOID
);

// FindDevices
/// Find PCI devices
/// @param VendorId  The vendor id of the PCI device
/// @param DeviceId  The device id of the PCI device
/// @param BaseClass The base class of the PCI device
/// @param SubClass  The sub class of the PCI device
/// @param Options   The options for matching the PCI devices
/// @param Count     On output, the count of PCI devices
/// @param Devices   On output, the PCI devices, which needs freed
/// @return Whether any devices were found or not
/// @retval EFI_INVALID_PARAMETER If Count or Devices is NULL or *Devices is not NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_NOT_FOUND         If no devices were found
/// @retval EFI_SUCCESS           If devices were found successfully
STATIC EFI_STATUS
EFIAPI
FindDevices (
  IN  UINT16                 VendorId,
  IN  UINT16                 DeviceId,
  IN  UINT8                  BaseClass,
  IN  UINT8                  SubClass,
  IN  UINTN                  Options,
  OUT UINTN                 *Count,
  OUT EFI_PCI_IO_PROTOCOL ***Devices
);
// FindDevicesById
/// Find PCI devices by base and sub classes
/// @param VendorId  The vendor id of the PCI device
/// @param DeviceId  The device id of the PCI device
/// @param Count     On output, the count of PCI devices
/// @param Devices   On output, the PCI devices, which needs freed
/// @return Whether any devices were found or not
/// @retval EFI_INVALID_PARAMETER If Count or Devices is NULL or *Devices is not NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_NOT_FOUND         If no devices were found
/// @retval EFI_SUCCESS           If devices were found successfully
EFI_STATUS
EFIAPI
FindDevicesById (
  IN  UINT16                 VendorId,
  IN  UINT16                 DeviceId,
  OUT UINTN                 *Count,
  OUT EFI_PCI_IO_PROTOCOL ***Devices
);
// FindDevicesByVendorId
/// Find PCI devices by base classes
/// @param VendorId The vendor id of the PCI device
/// @param Count    On output, the count of PCI devices
/// @param Devices  On output, the PCI devices, which needs freed
/// @return Whether any devices were found or not
/// @retval EFI_INVALID_PARAMETER If Count or Devices is NULL or *Devices is not NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_NOT_FOUND         If no devices were found
/// @retval EFI_SUCCESS           If devices were found successfully
EFI_STATUS
EFIAPI
FindDevicesByVendorId (
  IN  UINT16                 VendorId,
  OUT UINTN                 *Count,
  OUT EFI_PCI_IO_PROTOCOL ***Devices
);
// FindDevicesByClass
/// Find PCI devices by base and sub classes
/// @param BaseClass The base class of the PCI device
/// @param SubClass  The sub class of the PCI device
/// @param Count     On output, the count of PCI devices
/// @param Devices   On output, the PCI devices, which needs freed
/// @return Whether any devices were found or not
/// @retval EFI_INVALID_PARAMETER If Count or Devices is NULL or *Devices is not NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_NOT_FOUND         If no devices were found
/// @retval EFI_SUCCESS           If devices were found successfully
EFI_STATUS
EFIAPI
FindDevicesByClass (
  IN  UINT8                  BaseClass,
  IN  UINT8                  SubClass,
  OUT UINTN                 *Count,
  OUT EFI_PCI_IO_PROTOCOL ***Devices
);
// FindDevicesByBaseClass
/// Find PCI devices by base classes
/// @param BaseClass The base class of the PCI device
/// @param Count     On output, the count of PCI devices
/// @param Devices   On output, the PCI devices, which needs freed
/// @return Whether any devices were found or not
/// @retval EFI_INVALID_PARAMETER If Count or Devices is NULL or *Devices is not NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_NOT_FOUND         If no devices were found
/// @retval EFI_SUCCESS           If devices were found successfully
EFI_STATUS
EFIAPI
FindDevicesByBaseClass (
  IN  UINT8                  BaseClass,
  OUT UINTN                 *Count,
  OUT EFI_PCI_IO_PROTOCOL ***Devices
);

#endif // __CPU_INFORMATION_LIBRARY_HEADER__
