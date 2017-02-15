///
/// @file Library/PlatformLib/Devices.c
///
/// Memory information
///

#include "Platform.h"

#include <IndustryStandard/Pci.h>

#include <Library/UefiBootServicesTableLib.h>

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
) {
  EFI_STATUS            Status;
  EFI_HANDLE           *Handles = NULL;
  EFI_PCI_IO_PROTOCOL **FoundDevices = NULL;
  EFI_PCI_IO_PROTOCOL  *PciIo;
  UINTN                 DeviceCount = 0;
  UINTN                 DeviceIndex = 0;
  UINTN                 Index;
  UINT16                Ids[2];
  UINT8                 Class[2];

  // Check parameters
  if ((Count == NULL) || (Devices == NULL) || (*Devices != NULL) || (Options == 0)) {
    return EFI_INVALID_PARAMETER;
  }
  // Get all PCI devices
  LOG2(L"Find devices:", L"0x%04X, 0x%04X, 0x%02X, 0x%02X, 0x%0X\n", VendorId, DeviceId, BaseClass, SubClass, Options);
  Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiPciIoProtocolGuid, NULL, &DeviceCount, &Handles);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  if (Handles == NULL) {
    return EFI_NOT_FOUND;
  }
  if (DeviceCount == 0) {
    FreePool(Handles);
    return EFI_NOT_FOUND;
  }
  // Allocate array for devices
  FoundDevices = (EFI_PCI_IO_PROTOCOL **)AllocateZeroPool(DeviceCount * sizeof(EFI_PCI_IO_PROTOCOL *));
  if (FoundDevices == NULL) {
    FreePool(Handles);
    return EFI_OUT_OF_RESOURCES;
  }
  // Iterate through each PCI device
  LOG2(L"Devices:", L"%u\n", DeviceCount);
  for (Index = 0; Index < DeviceCount; ++Index) {
    if (Handles[Index] == NULL) {
      continue;
    }
    // Get the PCI IO protocol
    PciIo = NULL;
    if (EFI_ERROR(gBS->HandleProtocol(Handles[Index], &gEfiPciIoProtocolGuid, &PciIo))) {
      continue;
    }
    if (PciIo == NULL) {
      continue;
    }
    // Read vendor id and device id from offset 0
    Ids[0] = Ids[1] = 0;
    if (EFI_ERROR(PciIo->Pci.Read(PciIo, EfiPciIoWidthUint16, 0, ARRAY_SIZE(Ids), Ids))) {
      continue;
    }
    // Read base and sub classes from offset 0xA
    Class[0] = Class[1] = 0;
    if (EFI_ERROR(PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, 0xA, ARRAY_SIZE(Class), Class))) {
      continue;
    }
    LOG3(LOG_PREFIX_WIDTH - LOG(L"  Device(%u)", Index), L":", L"0x%04X, 0x%04X, 0x%02X, 0x%02X\n", Ids[0], Ids[1], Class[1], Class[0]);
    // Check for vendor id match
    if (((Options & FIND_DEVICE_VENDOR_ID) != 0) && (Ids[0] != VendorId)) {
      continue;
    }
    // Check for device id match
    if (((Options & FIND_DEVICE_DEVICE_ID) != 0) && (Ids[1] != DeviceId)) {
      continue;
    }
    // Check for base class match
    if (((Options & FIND_DEVICE_BASE_CLASS) != 0) && (Class[1] != BaseClass)) {
      continue;
    }
    // Check for sub class match
    if (((Options & FIND_DEVICE_SUB_CLASS) != 0) && (Class[0] != SubClass)) {
      continue;
    }
    // Increment the count of matching devices
    FoundDevices[DeviceIndex++] = PciIo;
  }
  FreePool(Handles);
  LOG2(L"  Count:", L"%u\n", DeviceIndex);
  if (DeviceIndex == 0) {
    // No devices not found
    FreePool(FoundDevices);
    return EFI_NOT_FOUND;
  }
  // Return the devices
  *Count = DeviceIndex;
  *Devices = FoundDevices;
  return EFI_SUCCESS;
}
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
) {
  return FindDevices(VendorId, DeviceId, 0, 0, FIND_DEVICE_VENDOR_ID | FIND_DEVICE_DEVICE_ID, Count, Devices);
}
// FindDevicesByVendorId
/// Find PCI devices by base classes
/// @param VendorId The vendor id of the PCI device
/// @param Count    On output, the count of PCI devices
/// @param Devices  On output, the PCI devices
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
) {
  return FindDevices(VendorId, 0, 0, 0, FIND_DEVICE_VENDOR_ID, Count, Devices);
}
// FindDevicesByClass
/// Find PCI devices by base and sub classes
/// @param BaseClass The base class of the PCI device
/// @param SubClass  The sub class of the PCI device
/// @param Count     On output, the count of PCI devices
/// @param Devices   On output, the PCI devices
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
) {
  return FindDevices(0, 0, BaseClass, SubClass, FIND_DEVICE_BASE_CLASS | FIND_DEVICE_SUB_CLASS, Count, Devices);
}
// FindDevicesByBaseClass
/// Find PCI devices by base classes
/// @param BaseClass The base class of the PCI device
/// @param Count     On output, the count of PCI devices
/// @param Devices   On output, the PCI devices
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
) {
  return FindDevices(0, 0, BaseClass, 0, FIND_DEVICE_BASE_CLASS, Count, Devices);
}
