;==============================================================================
;
; @file Build/Support/Boot/bootiso.s
;
;  Detect architecture (IA32 or X64), load architecture-specific legacy
;   firmware (BOOTIA32 or BOOTX64) or default legacy firmware (BOOT)
;   from El Torito legacy VFAT 12/16/32 boot volume
;
;------------------------------------------------------------------------------

;------------------------------------------------------------------------------
; Print extra debug information, this will probably inflate the image
;  beyond one physical sector which may make it not bootable on some machines
;------------------------------------------------------------------------------

; %define DEBUG

;==============================================================================
; Boot loader constants
;------------------------------------------------------------------------------

  ;----------------------------------------------------------------------------
  ; The load address of the boot loader (0000:7C00)
  ;----------------------------------------------------------------------------
  cLoadAddress        equ  0x7C00
  ;----------------------------------------------------------------------------
  ; The relocated base address of the boot loader (0000:7C00)
  ;----------------------------------------------------------------------------
  cBaseAddress        equ  0x7C00
  ;----------------------------------------------------------------------------
  ; The address of the top of the stack
  ;----------------------------------------------------------------------------
  cStackAddress       equ  cBaseAddress

  ;----------------------------------------------------------------------------
  ; The segment of the next stage boot loader (2000:XXXX)
  ;----------------------------------------------------------------------------
  cBootSegment        equ  0x2000
  ;----------------------------------------------------------------------------
  ; The address of the next stage boot loader (XXXX:0200)
  ;----------------------------------------------------------------------------
  cBootAddress        equ  0x0200

  ;----------------------------------------------------------------------------
  ; The sector size in bytes (2048 bytes)
  ;----------------------------------------------------------------------------
  cDefaultSectorSize  equ  0x0800

  ;----------------------------------------------------------------------------
  ; The size of the temporary read buffer (32 bytes)
  ;----------------------------------------------------------------------------
  cBufferSize         equ  0x20

;==============================================================================
; Boot loader name constants
;------------------------------------------------------------------------------

  ;----------------------------------------------------------------------------
  ; The first four characters, first half of name: 'BOOT'
  ;----------------------------------------------------------------------------
  cBootLoaderName1     equ  0x544F4F42
  ;----------------------------------------------------------------------------
  ; The second four characters, second half of name: '    '
  ;----------------------------------------------------------------------------
  cBootLoaderName2     equ  0x20202020
  ;----------------------------------------------------------------------------
  ; The second four characters for IA32, second half of name: 'IA32'
  ;----------------------------------------------------------------------------
  cBootLoaderNameIA32  equ  0x32334149
  ;----------------------------------------------------------------------------
  ; The second four characters for IA32, second half of name: 'X64 '
  ;----------------------------------------------------------------------------
  cBootLoaderNameX64   equ  0x20343658
  ;----------------------------------------------------------------------------
  ; The last four characters, name extension and null terminator: '   \0'
  ;----------------------------------------------------------------------------
  cBootLoaderName3     equ  0x00202020

;==============================================================================
; Boot volume constants
;------------------------------------------------------------------------------

  ;----------------------------------------------------------------------------
  ; The offset of the logical sector size for boot volume (2 bytes)
  ;----------------------------------------------------------------------------
  cVolumeSectorSize           equ  0x0B
  ;----------------------------------------------------------------------------
  ; The offset of the cluster logical sector count for boot volume (1 byte)
  ;----------------------------------------------------------------------------
  cVolumeClusterSectorCount   equ  0x0D
  ;----------------------------------------------------------------------------
  ; The offset of the reserved logical sector count for boot volume (2 bytes)
  ;----------------------------------------------------------------------------
  cVolumeReservedSectorCount  equ  0x0E
  ;----------------------------------------------------------------------------
  ; The offset of the count of tables for boot volume (1 byte)
  ;----------------------------------------------------------------------------
  cVolumeTableCount           equ  0x10
  ;----------------------------------------------------------------------------
  ; The offset of the count of root entries for boot volume (2 bytes)
  ;----------------------------------------------------------------------------
  cVolumeRootEntryCount       equ  0x11
  ;----------------------------------------------------------------------------
  ; The offset of the count of logical sectors for boot volume (2 bytes)
  ;----------------------------------------------------------------------------
  cVolumeSectorCount          equ  0x13
  ;----------------------------------------------------------------------------
  ; The offset of the table logical sector count for boot volume (2 bytes)
  ;----------------------------------------------------------------------------
  cVolumeTableSectorCount     equ  0x16
  ;----------------------------------------------------------------------------
  ; The offset of the extended logical sector count for boot
  ;  volume (4 bytes), if logical sector count is zero
  ;----------------------------------------------------------------------------
  cVolumeSectorCountEx        equ  0x20
  ;----------------------------------------------------------------------------
  ; The offset of the extended table logical sector count for boot
  ;  volume (4 bytes), if table logical sector count is zero
  ;----------------------------------------------------------------------------
  cVolumeTableSectorCountEx   equ  0x24
  ;----------------------------------------------------------------------------
  ; The offset of the root directory cluster for boot
  ;  volume (4 bytes), if root entry count is zero
  ;----------------------------------------------------------------------------
  cVolumeRootCluster          equ  0x2C

  ;----------------------------------------------------------------------------
  ; The size of a root entry for boot volume (32 bytes)
  ;----------------------------------------------------------------------------
  cVolumeEntrySize            equ  0x20
  ;----------------------------------------------------------------------------
  ; The offset of the high word of cluster (2 bytes)
  ;----------------------------------------------------------------------------
  cVolumeEntryHighCluster     equ  0x14
  ;----------------------------------------------------------------------------
  ; The offset of the low word of cluster (2 bytes)
  ;----------------------------------------------------------------------------
  cVolumeEntryLowCluster      equ  0x1A

;==============================================================================
; Drive constants
;------------------------------------------------------------------------------

  ;----------------------------------------------------------------------------
  ; The offset of the flags for the drive (2 bytes)
  ;----------------------------------------------------------------------------
  cDriveFlags          equ  0x02
  ;----------------------------------------------------------------------------
  ; The offset of the cylinder count for the drive (4 bytes)
  ;----------------------------------------------------------------------------
  cDriveCylinderCount  equ  0x04
  ;----------------------------------------------------------------------------
  ; The offset of the head count for the drive (4 bytes)
  ;----------------------------------------------------------------------------
  cDriveHeadCount      equ  0x08
  ;----------------------------------------------------------------------------
  ; The offset of the sector count for the drive (4 bytes)
  ;----------------------------------------------------------------------------
  cDriveSectorCount    equ  0x0C
  ;----------------------------------------------------------------------------
  ; The offset of the sector size for the drive (2 bytes)
  ;----------------------------------------------------------------------------
  cDriveSectorSize     equ  0x18

;==============================================================================
; CDROM constants
;------------------------------------------------------------------------------

  ;----------------------------------------------------------------------------
  ; The offset of the cylinder count for the CDROM (1 byte)
  ;----------------------------------------------------------------------------
  cCDROMCylinderCount  equ  0x10
  ;----------------------------------------------------------------------------
  ; The offset of the sector count for the CDROM (1 byte)
  ;----------------------------------------------------------------------------
  cCDROMSectorCount    equ  0x11
  ;----------------------------------------------------------------------------
  ; The offset of the head count for the CDROM (1 byte)
  ;----------------------------------------------------------------------------
  cCDROMHeadCount      equ  0x12

;==============================================================================
; BIOS reset vector constants
;------------------------------------------------------------------------------

  ;----------------------------------------------------------------------------
  ; The reset vector jump segment
  ;----------------------------------------------------------------------------
  cResetSegment  equ  0xF000
  ;----------------------------------------------------------------------------
  ; The reset vector jump address
  ;----------------------------------------------------------------------------
  cResetAddress  equ  0xFFF0
  ;----------------------------------------------------------------------------
  ; The BIOS data area reset flag address
  ;----------------------------------------------------------------------------
  cResetFlag     equ  0x0472

;==============================================================================
; Feature constants
;------------------------------------------------------------------------------

  ;----------------------------------------------------------------------------
  ; X64 architecture feature
  ;----------------------------------------------------------------------------
  cFeatureX64                  equ  0x01
  ;----------------------------------------------------------------------------
  ; Extended read/write int 0x13 feature
  ;----------------------------------------------------------------------------
  cFeatureExtRW                equ  0x02
  ;----------------------------------------------------------------------------
  ; Logical sector needs divided instead of multiplied to get physical sector
  ;----------------------------------------------------------------------------
  cFeatureLargerLogicalSector  equ  0x04

;==============================================================================
; El Torito boot information
;------------------------------------------------------------------------------

struc BootInfo

    ;--------------------------------------------------------------------------
    ; Physical sector LBA of primary volume descriptor
    ;--------------------------------------------------------------------------
    .Descriptor:  resd  1
    ;--------------------------------------------------------------------------
    ; Starting physical sector LBA of boot loader
    ;--------------------------------------------------------------------------
    .BootStart:   resd  1
    ;--------------------------------------------------------------------------
    ; Size in bytes of boot loader and boot volume
    ;--------------------------------------------------------------------------
    .TotalSize:   resd  1
    ;--------------------------------------------------------------------------
    ; Checksum of boot loader and boot volume
    ;--------------------------------------------------------------------------
    .Checksum:    resd  1

    ;--------------------------------------------------------------------------
    ; Physical sector count of boot loader
    ;--------------------------------------------------------------------------
    .BootCount:    resd  1
    ;--------------------------------------------------------------------------
    ; Starting physical sector LBA of boot volume
    ;--------------------------------------------------------------------------
    .VolumeStart:  resd  1
    ;--------------------------------------------------------------------------
    ; Physical sector count of boot volume
    ;--------------------------------------------------------------------------
    .VolumeCount:  resd  1
    ;--------------------------------------------------------------------------
    ; Physical sector count of boot loader and boot volume
    ;--------------------------------------------------------------------------
    .Count:        resd  1

    ;--------------------------------------------------------------------------
    ; Physical sector size
    ;--------------------------------------------------------------------------
    .SectorSize:       resd  1
    ;--------------------------------------------------------------------------
    ; Physical sectors per track of physical volume
    ;--------------------------------------------------------------------------
    .SectorsPerTrack:  resd  1
    ;--------------------------------------------------------------------------
    ; Physical cylinders of physical volume
    ;--------------------------------------------------------------------------
    .Cylinders:        resd  1
    ;--------------------------------------------------------------------------
    ; Physical heads of physical volume
    ;--------------------------------------------------------------------------
    .Heads:            resd  1

    ;--------------------------------------------------------------------------
    ; BIOS boot entry pointer address converted from DS:SI
    ;--------------------------------------------------------------------------
    .BootEntry:  resd  1
    ;--------------------------------------------------------------------------
    ; BIOS drive identifier from DL
    ;--------------------------------------------------------------------------
    .BootDrive:  resb  1
    ;--------------------------------------------------------------------------
    ; BIOS boot flags
    ;--------------------------------------------------------------------------
    .BootFlags:  resb  1

    ;--------------------------------------------------------------------------
    ; Size of the structure (50 bytes)
    ;--------------------------------------------------------------------------
    .Size:

endstruc

;==============================================================================
; Boot data
;------------------------------------------------------------------------------

struc BootData

    ;--------------------------------------------------------------------------
    ; The address of the volume table buffer
    ;--------------------------------------------------------------------------
    .Table:      resd  1
    ;--------------------------------------------------------------------------
    ; The size in bytes of the volume table buffer
    ;--------------------------------------------------------------------------
    .TableSize:  resd  1

    ;--------------------------------------------------------------------------
    ; The address of the volume root directory buffer
    ;--------------------------------------------------------------------------
    .RootDirectory:      resd  1
    ;--------------------------------------------------------------------------
    ; The size in bytes of the volume root directory buffer
    ;--------------------------------------------------------------------------
    .RootDirectorySize:  resd  1

    ;--------------------------------------------------------------------------
    ; A breakpoint counter
    ;--------------------------------------------------------------------------
    .Breakpoint:  resd  1

    ;--------------------------------------------------------------------------
    ; The logical sector size for the boot volume
    ;--------------------------------------------------------------------------
    .SectorSize:     resd  1

    ;--------------------------------------------------------------------------
    ; The count of reserved logical sectors on the boot volume
    ;--------------------------------------------------------------------------
    .ReservedCount:  resd  1

    ;--------------------------------------------------------------------------
    ; The count of logical sectors per cluster table on the boot volume
    ;--------------------------------------------------------------------------
    .SectorsPerTable:  resd  1
    ;--------------------------------------------------------------------------
    ; The count of cluster tables on the boot volume
    ;--------------------------------------------------------------------------
    .TableCount:       resd  1

    ;--------------------------------------------------------------------------
    ; The count of root entries on the boot volume
    ;--------------------------------------------------------------------------
    .RootEntryCount:        resd  1
    ;--------------------------------------------------------------------------
    ; The cluster of the root directory of the boot volume
    ;--------------------------------------------------------------------------
    .RootDirectoryCluster:  resd  1
    ;--------------------------------------------------------------------------
    ; The logical sector count of the root directory of the boot volume
    ;--------------------------------------------------------------------------
    .RootDirectoryCount:    resd  1

    ;--------------------------------------------------------------------------
    ; The logical sectors per cluster for the boot volume
    ;--------------------------------------------------------------------------
    .SectorsPerCluster:  resd  1
    ;--------------------------------------------------------------------------
    ; The cluster count of the boot volume
    ;--------------------------------------------------------------------------
    .ClusterCount:       resd  1

    ;--------------------------------------------------------------------------
    ; Starting logical sector of data area (clusters)
    ;--------------------------------------------------------------------------
    .Start:  resd  1
    ;--------------------------------------------------------------------------
    ; The logical sector count of the boot volume
    ;--------------------------------------------------------------------------
    .Count:  resd  1

    ;--------------------------------------------------------------------------
    ; The temporary read buffer
    ;--------------------------------------------------------------------------
    .Buffer:  resb  cBufferSize

    ;--------------------------------------------------------------------------
    ; Size of the structure (64 bytes + cBufferSize bytes)
    ;--------------------------------------------------------------------------
    .Size:

endstruc

;==============================================================================
; Start of text segment
;------------------------------------------------------------------------------

  SEGMENT .text

    ;--------------------------------------------------------------------------
    ; Set the instruction set as 16bit real mode
    ;--------------------------------------------------------------------------
    BITS 16

    ;--------------------------------------------------------------------------
    ; Set the instruction set to use as i586
    ;--------------------------------------------------------------------------
    CPU 586

    ;--------------------------------------------------------------------------
    ; Set where boot code is loaded
    ;--------------------------------------------------------------------------
    ORG cBaseAddress

;==============================================================================
; Entry point
;==============================================================================

start:

    ;--------------------------------------------------------------------------
    ; Clear interrupts
    ;--------------------------------------------------------------------------
    cli
    ;--------------------------------------------------------------------------
    ; Clear or set direction flag
    ;--------------------------------------------------------------------------

%if cLoadAddress >= cBaseAddress

    cld

%elif

    std

%endif

    ;--------------------------------------------------------------------------
    ; Clear EAX here to save space later
    ;--------------------------------------------------------------------------
    xor       eax, eax
    ;--------------------------------------------------------------------------
    ; Jump to the loader start
    ;--------------------------------------------------------------------------
    jmp       loader_start

    ;--------------------------------------------------------------------------
    ; The El Torito boot information starts at offset 8 so pad with zeros
    ;--------------------------------------------------------------------------
    times     (8-($-$$))  db  0

    ;--------------------------------------------------------------------------
    ; El Torito boot information structure
    ;--------------------------------------------------------------------------

  boot_info:

    istruc BootInfo

      at BootInfo.Descriptor,       dd  0
      at BootInfo.BootStart,        dd  0
      at BootInfo.TotalSize,        dd  0
      at BootInfo.Checksum,         dd  0
      at BootInfo.BootCount,        dd  0
      at BootInfo.VolumeStart,      dd  0
      at BootInfo.VolumeCount,      dd  0
      at BootInfo.Count,            dd  0
      at BootInfo.SectorSize,       dd  0
      at BootInfo.SectorsPerTrack,  dd  0
      at BootInfo.Cylinders,        dd  0
      at BootInfo.Heads,            dd  0
      at BootInfo.BootEntry,        dd  0
      at BootInfo.BootDrive,        db  0
      at BootInfo.BootFlags,        db  0

    iend

    ;--------------------------------------------------------------------------
    ; The El Torito boot information ends at offset 64 so pad with zeros
    ;--------------------------------------------------------------------------
    times     (64-($-$$))  db  0

;==============================================================================
; Loader start
;
; @param DL    The BIOS drive identifier
; @param DS:SI The BIOS boot entry pointer (valid?)
;==============================================================================

loader_start:

    ;--------------------------------------------------------------------------
    ; Setup the stack segment and address
    ;--------------------------------------------------------------------------
    mov       ss, ax
    mov       esp, dword cStackAddress

    ;--------------------------------------------------------------------------
    ; Save DL for BIOS drive identifier
    ;--------------------------------------------------------------------------
    mov       byte [cs:(cLoadAddress+(boot_info-cBaseAddress))+BootInfo.BootDrive], dl
    ;--------------------------------------------------------------------------
    ; Save DS:SI for BIOS boot entry address
    ;--------------------------------------------------------------------------
    mov       ax, ds
    shl       eax, 4
    and       esi, dword 0xFFFF
    add       eax, esi
    mov       dword [cs:((cLoadAddress+(boot_info-cBaseAddress))+BootInfo.BootEntry)], eax

    ;--------------------------------------------------------------------------
    ; Restore interrupts
    ;--------------------------------------------------------------------------
    sti

%if cLoadAddress != cBaseAddress

    ;--------------------------------------------------------------------------
    ; Set the destination and source addresses
    ;--------------------------------------------------------------------------
    mov       ecx, dword [cs:(cLoadAddress+(boot_size-cBaseAddress))]
    push      word (cLoadAddress >> 4)
    pop       ds
    mov       esi, dword (cLoadAddress & 0xF)
    push      word (cBaseAddress >> 4)
    pop       es
    mov       edi, dword (cBaseAddress & 0xF)

    ;--------------------------------------------------------------------------
    ; Relocate the boot loader
    ;--------------------------------------------------------------------------
    rep movsb

 %if cLoadAddress < cBaseAddress

    ;--------------------------------------------------------------------------
    ; Clear direction flag DF if needed
    ;--------------------------------------------------------------------------
    cld

 %endif

    ;--------------------------------------------------------------------------
    ; Jump to the relocated boot loader
    ;--------------------------------------------------------------------------
    jmp       0:loader_relocated

loader_relocated:

%endif

;==============================================================================
; Loader relocated
;==============================================================================

    ;--------------------------------------------------------------------------
    ; Get boot features
    ;--------------------------------------------------------------------------
    call      get_features
    ;--------------------------------------------------------------------------
    ; Get boot information
    ;--------------------------------------------------------------------------
    call      get_info
    ;--------------------------------------------------------------------------
    ; Get boot volume information
    ;--------------------------------------------------------------------------
    call      get_volume_info

    ;--------------------------------------------------------------------------
    ; Setup boot volume cluster table buffer
    ;--------------------------------------------------------------------------
    xor       edx, edx
    mov       eax, dword [cs:(boot_data+BootData.SectorsPerTable)]
    mul       dword [cs:(boot_data+BootData.SectorSize)]
    mov       dword [cs:(boot_data+BootData.TableSize)], eax
    mov       edi, dword (boot_data+BootData.Buffer+cBufferSize)
    mov       dword [cs:(boot_data+BootData.Table)], edi

    ;--------------------------------------------------------------------------
    ; Load boot volume cluster table
    ;--------------------------------------------------------------------------
    mov       ebx, dword [cs:(boot_data+BootData.ReservedCount)]
    mov       ecx, dword [cs:(boot_data+BootData.SectorsPerTable)]
    call      read_logical_sectors

    ;--------------------------------------------------------------------------
    ; Setup boot volume root entry buffer
    ;--------------------------------------------------------------------------
    mov       dword [cs:(boot_data+BootData.Table)], edi
    add       edi, dword [cs:(boot_data+BootData.TableSize)]
    mov       dword [cs:(boot_data+BootData.RootDirectory)], edi

    ;--------------------------------------------------------------------------
    ; Check there is a static root directory if bv_root_count != 0
    ;--------------------------------------------------------------------------
    mov       ebx, dword [cs:(boot_data+BootData.RootEntryCount)]
    or        ebx, ebx
    jz        .load_volume_root
    ;--------------------------------------------------------------------------
    ; Load boot volume root entry buffer
    ;--------------------------------------------------------------------------
    shl       ebx, 5
    mov       dword [cs:(boot_data+BootData.RootDirectorySize)], ebx
    mov       ebx, dword [cs:(boot_data+BootData.Start)]
    mov       ecx, dword [cs:(boot_data+BootData.RootDirectoryCount)]
    sub       ebx, ecx
    call      read_logical_sectors
    jmp       .find_boot_cluster

  .load_volume_root:

    ;--------------------------------------------------------------------------
    ; Load boot volume root directory from cluster
    ;--------------------------------------------------------------------------

    mov       ebx, dword [cs:(boot_data+BootData.RootDirectoryCluster)]
    call      load_clusters
    mov       dword [cs:(boot_data+BootData.RootDirectorySize)], ecx

  .find_boot_cluster:

    ;--------------------------------------------------------------------------
    ; Print boot information
    ;--------------------------------------------------------------------------

%ifdef DEBUG

    call      print_info

%endif

    ;--------------------------------------------------------------------------
    ; Find the next stage architecture boot loader cluster from boot volume
    ;--------------------------------------------------------------------------
    mov       esi, dword (boot_data+BootData.Buffer)
    mov       dword [cs:(boot_data+BootData.Buffer)], dword cBootLoaderName1
    mov       dword [cs:(boot_data+BootData.Buffer+4)], dword cBootLoaderNameIA32
    mov       dword [cs:(boot_data+BootData.Buffer+8)], dword cBootLoaderName3
    mov       al, byte [cs:(boot_info+BootInfo.BootFlags)]
    and       al, byte cFeatureX64
    jz        .find_arch_boot_cluster

    ;--------------------------------------------------------------------------
    ; Find BOOTX64
    ;--------------------------------------------------------------------------
    mov       dword [cs:(boot_data+BootData.Buffer+4)], dword cBootLoaderNameX64

  .find_arch_boot_cluster:

%ifdef DEBUG

    call      print_string
    push      esi
    mov       esi, dword str_new_line
    call      print_string
    pop       esi

%endif

    call      find_boot_cluster

    ;--------------------------------------------------------------------------
    ; Check the architecture boot loader cluster was found
    ;--------------------------------------------------------------------------
    or        ebx, ebx
    jnz       .boot_cluster_found

    ;--------------------------------------------------------------------------
    ; Find the next stage boot loader cluster from boot volume
    ;--------------------------------------------------------------------------
    mov       dword [cs:(boot_data+BootData.Buffer+4)], dword cBootLoaderName2

%ifdef DEBUG

    call      print_string
    push      esi
    mov       esi, dword str_new_line
    call      print_string
    pop       esi

%endif

    call      find_boot_cluster

    ;--------------------------------------------------------------------------
    ; Check the boot loader cluster was found
    ;--------------------------------------------------------------------------
    or        ebx, ebx
    jnz       .boot_cluster_found

    ;--------------------------------------------------------------------------
    ; The boot loader cluster was not found
    ;--------------------------------------------------------------------------
    mov       esi, dword str_not_found
    jmp       boot_fail

  .boot_cluster_found:

%ifdef DEBUG

    ;--------------------------------------------------------------------------
    ; Print boot volume boot loader cluster
    ;--------------------------------------------------------------------------
    mov       esi, dword str_boot_cluster
    mov       edx, ebx
    call      log_hex

%endif

    ;--------------------------------------------------------------------------
    ; Load next stage boot loader
    ;--------------------------------------------------------------------------

    mov       edi, dword ((cBootSegment << 4) + cBootAddress)
    call      load_clusters
    or        ecx, ecx
    jnz       .jump_next_boot
    mov       esi, dword str_not_found
    jmp       boot_fail
    
  .jump_next_boot:

    ;--------------------------------------------------------------------------
    ; Jump to next stage boot loader
    ;--------------------------------------------------------------------------
    mov       esi, dword [cs:(boot_info+BootInfo.BootEntry)]
    mov       eax, esi
    shr       eax, 4
    mov       ds, ax
    and       esi, dword 0xF
    mov       dl, byte [cs:(boot_info+BootInfo.BootDrive)]
    jmp       cBootSegment:cBootAddress

    ;--------------------------------------------------------------------------
    ; Should never get here but if so, print failure message and restart
    ;--------------------------------------------------------------------------

;==============================================================================
; Unknown boot failure, wait for key press then restart
;==============================================================================

boot_failure:

    mov       esi, dword str_failure

;==============================================================================
; Boot failed, wait for key press then restart
;
; @param ESI The failure message to log
;==============================================================================

boot_fail:

    call      print_string
    mov       esi, dword str_new_line
    call      print_string

    ;--------------------------------------------------------------------------
    ; Print the restart notification message
    ;--------------------------------------------------------------------------
    mov       esi, dword str_restart
    call      print_string

    ;--------------------------------------------------------------------------
    ; Wait for any key press
    ;--------------------------------------------------------------------------
    call      read_char

    ;--------------------------------------------------------------------------
    ; Restart
    ;--------------------------------------------------------------------------
    mov       word [cs:cResetFlag], word 0
    jmp       cResetSegment:cResetAddress

;==============================================================================
; Get boot features
;==============================================================================

get_features:

    ;--------------------------------------------------------------------------
    ; Write to EFLAGS bit 21 for CPUID execution
    ;--------------------------------------------------------------------------
    pushfd
    pop       eax
    or        eax, dword 0x200000
    push      eax
    popfd

    ;--------------------------------------------------------------------------
    ; Test if EFLAGS bit 21 is still set,
    ;  if so then CPUID instruction is available
    ;--------------------------------------------------------------------------
    pushfd
    pop       eax
    and       eax, dword 0x200000
    jz        .get_feature_ext_rw

    ;--------------------------------------------------------------------------
    ; CPUID is executable
    ;--------------------------------------------------------------------------
    mov       eax, dword 0x80000000
    cpuid
    cmp       eax, dword 0x80000001
    jb        .get_feature_ext_rw

    ;--------------------------------------------------------------------------
    ; Check for X64 architecture
    ;--------------------------------------------------------------------------
    xor       edx, edx
    mov       eax, dword 0x80000001
    cpuid
    ;--------------------------------------------------------------------------
    ; EDX bit 29 is set for X64 architecture support
    ;--------------------------------------------------------------------------
    and       edx, dword 0x20000000
    jz        .get_feature_ext_rw
    ;--------------------------------------------------------------------------
    ; Set X64 feature bit
    ;--------------------------------------------------------------------------
    or        byte [cs:(boot_info+BootInfo.BootFlags)], byte cFeatureX64

  .get_feature_ext_rw:

    ;--------------------------------------------------------------------------
    ; Check read/write extensions are present
    ;--------------------------------------------------------------------------
    mov       dl, byte [cs:(boot_info+BootInfo.BootDrive)]
    mov       ah, byte 0x41
    mov       bx, word 0x55AA
    int       0x13
    jc        .get_features_done
    and       cl, byte 1
    jz        .get_features_done
    ;--------------------------------------------------------------------------
    ; Set read/write extensions feature bit
    ;--------------------------------------------------------------------------
    or        byte [cs:(boot_info+BootInfo.BootFlags)], byte cFeatureExtRW

  .get_features_done:

    ret

;==============================================================================
; Get boot information
;==============================================================================

get_info:

    ;--------------------------------------------------------------------------
    ; Check read/write extensions feature is present
    ;--------------------------------------------------------------------------
    mov       dl, byte [cs:(boot_info+BootInfo.BootFlags)]
    and       dl, cFeatureExtRW
    jz        .old_get_info

    ;--------------------------------------------------------------------------
    ; Setup temporary read buffer for boot drive parameters
    ;--------------------------------------------------------------------------
    mov       esi, dword (boot_data+BootData.Buffer)
    mov       word [cs:(boot_data+BootData.Buffer)], word 0x1A
    ;--------------------------------------------------------------------------
    ; Get boot drive parameters
    ;--------------------------------------------------------------------------
    mov       dl, byte [cs:(boot_info+BootInfo.BootDrive)]
    mov       ah, byte 0x48
    int       0x13
    jc        .use_default_sector_size
    ;--------------------------------------------------------------------------
    ; Get boot drive sector size
    ;--------------------------------------------------------------------------
    mov       ax, word [cs:(boot_data+BootData.Buffer+cDriveSectorSize)]
    or        ax, ax
    jz        .use_default_sector_size
    mov       word [cs:(boot_info+BootInfo.SectorSize)], ax
    jmp       .cdrom_get_info

  .use_default_sector_size:
  
    ;--------------------------------------------------------------------------
    ; Set boot drive sector size to default sector size
    ;--------------------------------------------------------------------------
    mov       word [cs:(boot_info+BootInfo.SectorSize)], word cDefaultSectorSize

  .cdrom_get_info:

    ;--------------------------------------------------------------------------
    ; Get the drive parameters from CDROM emulation
    ;--------------------------------------------------------------------------
    mov       dl, byte [cs:(boot_info+BootInfo.BootDrive)]
    mov       ax, word 0x4B01
    mov       byte [cs:(boot_data+BootData.Buffer)], byte 0x13
    int       0x13
    jc        .old_get_info
    mov       dh, byte [cs:(boot_data+BootData.Buffer+cCDROMHeadCount)]
    mov       byte [cs:(boot_info+BootInfo.Heads)], dh
    inc       dword [cs:(boot_info+BootInfo.Heads)]
    mov       dh, byte [cs:(boot_data+BootData.Buffer+cCDROMSectorCount)]
    mov       byte [cs:(boot_info+BootInfo.SectorsPerTrack)], dh
    and       byte [cs:(boot_info+BootInfo.SectorsPerTrack)], 0x3F
    shr       dh, 6
    mov       dl, byte [cs:(boot_data+BootData.Buffer+cCDROMCylinderCount)]
    mov       word [cs:(boot_info+BootInfo.Cylinders)], dx
    inc       dword [cs:(boot_info+BootInfo.Cylinders)]
    jmp       .get_info

  .old_get_info:

    ;--------------------------------------------------------------------------
    ; Set boot drive sector size to default sector size
    ;--------------------------------------------------------------------------
    mov       word [cs:(boot_info+BootInfo.SectorSize)], word cDefaultSectorSize

    ;--------------------------------------------------------------------------
    ; Use old method to get drive parameters
    ;--------------------------------------------------------------------------
    mov       dl, byte [cs:(boot_info+BootInfo.BootDrive)]
    mov       ah, byte 0x08
    int       0x13
    jc        boot_failure

    ;--------------------------------------------------------------------------
    ; Get boot drive parameters using old methods
    ;--------------------------------------------------------------------------
    mov       byte [cs:(boot_info+BootInfo.Heads)], dh
    inc       dword [cs:(boot_info+BootInfo.Heads)]
    mov       dh, cl
    and       cl, 0x3F
    mov       byte [cs:(boot_info+BootInfo.SectorsPerTrack)], cl
    shr       dh, 6
    mov       dl, ch
    mov       word [cs:(boot_info+BootInfo.Cylinders)], dx
    inc       dword [cs:(boot_info+BootInfo.Cylinders)]
    jmp       .get_info

    ;--------------------------------------------------------------------------
    ; Failed to get boot drive parameters using any method
    ;--------------------------------------------------------------------------
    jmp       boot_failure

  .get_info:

    ;--------------------------------------------------------------------------
    ; Get boot drive sector count
    ;--------------------------------------------------------------------------
    xor       edx, edx
    mov       eax, dword [cs:(boot_info+BootInfo.TotalSize)]
    mov       ecx, dword [cs:(boot_info+BootInfo.SectorSize)]
    add       eax, ecx
    dec       eax
    div       ecx
    mov       dword [cs:(boot_info+BootInfo.Count)], eax

    ;--------------------------------------------------------------------------
    ; Get boot loader sector count
    ;--------------------------------------------------------------------------
    xor       edx, edx
    mov       eax, dword [cs:boot_size]
    add       eax, ecx
    dec       eax
    div       ecx
    mov       dword [cs:(boot_info+BootInfo.BootCount)], eax

    ;--------------------------------------------------------------------------
    ; Get boot volume sector start
    ;--------------------------------------------------------------------------
    mov       eax, dword [cs:(boot_info+BootInfo.BootStart)]
    add       eax, dword [cs:(boot_info+BootInfo.BootCount)]
    mov       dword [cs:(boot_info+BootInfo.VolumeStart)], eax

    ;--------------------------------------------------------------------------
    ; Get boot volume sector count
    ;--------------------------------------------------------------------------
    xor       edx, edx
    mov       eax, dword [cs:(boot_info+BootInfo.TotalSize)]
    add       eax, ecx
    dec       eax
    div       ecx
    sub       eax, dword [cs:(boot_info+BootInfo.BootCount)]
    mov       dword [cs:(boot_info+BootInfo.VolumeCount)], eax

    ret

;==============================================================================
; Get boot volume information
;==============================================================================

get_volume_info:

    ;--------------------------------------------------------------------------
    ; Setup the volume information buffer
    ;--------------------------------------------------------------------------
    mov       edi, dword (boot_data+BootData.Buffer+cBufferSize)
    ;--------------------------------------------------------------------------
    ; Load boot volume information
    ;--------------------------------------------------------------------------
    mov       ebx, dword [cs:(boot_info+BootInfo.VolumeStart)]
    mov       ecx, dword 1
    call      read_sectors

    ;--------------------------------------------------------------------------
    ; Setup segment for volume information
    ;--------------------------------------------------------------------------
    mov       eax, edi
    shr       eax, 4
    mov       es, ax
    and       edi, dword 0xF

    ;--------------------------------------------------------------------------
    ; Get the volume size in bytes
    ;--------------------------------------------------------------------------
    mov       eax, dword [cs:(boot_info+BootInfo.TotalSize)]
    cmp       eax, dword [cs:boot_size]
    ja        .loader_size_valid

    ;--------------------------------------------------------------------------
    ; Boot failed because the boot loader volume has no sectors
    ;--------------------------------------------------------------------------
    jmp       boot_failure

  .loader_size_valid:

    ;--------------------------------------------------------------------------
    ; Get the count of volume cluster tables
    ;--------------------------------------------------------------------------
    xor       eax, eax
    mov       al, byte [es:(edi+cVolumeTableCount)]
    mov       byte [cs:(boot_data+BootData.TableCount)], al

    ;--------------------------------------------------------------------------
    ; Get the cluster sector count
    ;--------------------------------------------------------------------------
    mov       al, byte [es:(edi+cVolumeClusterSectorCount)]
    mov       byte [cs:(boot_data+BootData.SectorsPerCluster)], al

    ;--------------------------------------------------------------------------
    ; Get the sector size of the volume
    ;--------------------------------------------------------------------------
    mov       ax, word [es:(edi+cVolumeSectorSize)]
    mov       word [cs:(boot_data+BootData.SectorSize)], ax

    ;--------------------------------------------------------------------------
    ; Get the reserved sector count of the volume
    ;--------------------------------------------------------------------------
    mov       ax, word [es:(edi+cVolumeReservedSectorCount)]
    mov       word [cs:(boot_data+BootData.ReservedCount)], ax

    ;--------------------------------------------------------------------------
    ; Get the sector count of a cluster table
    ;--------------------------------------------------------------------------
    mov       ax, word [es:(edi+cVolumeTableSectorCount)]
    or        ax, ax
    jnz       .valid_table_sector_count
    mov       eax, dword [es:(edi+cVolumeTableSectorCountEx)]

  .valid_table_sector_count:

    mov       dword [cs:(boot_data+BootData.SectorsPerTable)], eax

    ;--------------------------------------------------------------------------
    ; Get the count of root entries
    ;--------------------------------------------------------------------------
    mov       ax, word [es:(edi+cVolumeRootEntryCount)]
    mov       word [cs:(boot_data+BootData.RootEntryCount)], ax
    or        ax, ax
    jnz       .valid_root_count

    ;--------------------------------------------------------------------------
    ; Get the FAT root cluster (only valid if boot_data.RootEntryCount == 0)
    ;--------------------------------------------------------------------------
    mov       eax, dword [es:(edi+cVolumeRootCluster)]
    mov       dword [cs:(boot_data+BootData.RootDirectoryCluster)], eax

  .valid_root_count:

    ;--------------------------------------------------------------------------
    ; Get the starting logical sector of the data area
    ;--------------------------------------------------------------------------
    xor       ecx, ecx
    mov       cx, word [cs:(boot_data+BootData.ReservedCount)]

    ;--------------------------------------------------------------------------
    ; Add the logical sector count for the volume tables
    ;--------------------------------------------------------------------------
    xor       edx, edx
    mov       eax, dword [cs:(boot_data+BootData.SectorsPerTable)]
    mul       dword [cs:(boot_data+BootData.TableCount)]
    add       ecx, eax

    ;--------------------------------------------------------------------------
    ; Add the logical sector count for the root directory
    ;--------------------------------------------------------------------------
    xor       edx, edx
    mov       eax, dword [cs:(boot_data+BootData.RootEntryCount)]
    shr       eax, 4
    mov       dword [cs:(boot_data+BootData.RootDirectoryCount)], eax
    add       ecx, eax

    ;--------------------------------------------------------------------------
    ; Store the starting logical sector for the data area
    ;--------------------------------------------------------------------------
    mov       dword [cs:(boot_data+BootData.Start)], ecx

    ;--------------------------------------------------------------------------
    ; Get the logical sector count
    ;--------------------------------------------------------------------------
    xor       ecx, ecx
    mov       cx, word [es:(edi+cVolumeSectorCount)]
    or        ecx, ecx
    jnz       .valid_sector_count
    ;--------------------------------------------------------------------------
    ; Get the extended logical sector count since the previous was zero
    ;--------------------------------------------------------------------------
    mov       ecx, dword [es:(edi+cVolumeSectorCountEx)]

  .valid_sector_count:

    ;--------------------------------------------------------------------------
    ; Subtract the starting logical sector for the data area
    ;--------------------------------------------------------------------------
    sub       ecx, dword [cs:(boot_data+BootData.Start)]

    ;--------------------------------------------------------------------------
    ; Store the logical sector count for the boot volume
    ;--------------------------------------------------------------------------
    mov       dword [cs:(boot_data+BootData.Count)], ecx

    ;--------------------------------------------------------------------------
    ; Calculate the cluster count for the volume
    ;--------------------------------------------------------------------------
    xor       edx, edx
    mov       eax, ecx
    mov       ecx, dword [cs:(boot_data+BootData.SectorsPerCluster)]
    div       ecx

    ;--------------------------------------------------------------------------
    ; Store the cluster count for the volume
    ;--------------------------------------------------------------------------
    mov       dword [cs:(boot_data+BootData.ClusterCount)], eax

    ret

;==============================================================================
; Print boot information
;==============================================================================

%ifdef DEBUG

print_info:

    ;--------------------------------------------------------------------------
    ; Print boot volume starting physical sector
    ;--------------------------------------------------------------------------
    mov       esi, dword str_physical_start
    mov       edx, dword [cs:(boot_info+BootInfo.VolumeStart)]
    call      log_hex

    ;--------------------------------------------------------------------------
    ; Print boot volume physical sector count
    ;--------------------------------------------------------------------------
    mov       esi, dword str_physical_sector_count
    mov       edx, dword [cs:(boot_info+BootInfo.VolumeCount)]
    call      log_hex

    ;--------------------------------------------------------------------------
    ; Print boot volume physical sector size
    ;--------------------------------------------------------------------------
    mov       esi, dword str_physical_sector_size
    xor       edx, edx
    mov       dx, word [cs:(boot_info+BootInfo.SectorSize)]
    call      log_hex

    ;--------------------------------------------------------------------------
    ; Print boot volume starting logical sector for data area
    ;--------------------------------------------------------------------------
    mov       esi, dword str_logical_start
    xor       edx, edx
    mov       dx, word [cs:(boot_data+BootData.Start)]
    call      log_hex

    ;--------------------------------------------------------------------------
    ; Print boot volume logical sector count
    ;--------------------------------------------------------------------------
    mov       esi, dword str_logical_sector_count
    mov       edx, dword [cs:(boot_data+BootData.Count)]
    call      log_hex

    ;--------------------------------------------------------------------------
    ; Print boot volume logical sector size
    ;--------------------------------------------------------------------------
    mov       esi, dword str_logical_sector_size
    xor       edx, edx
    mov       dx, word [cs:(boot_data+BootData.SectorSize)]
    call      log_hex

    ;--------------------------------------------------------------------------
    ; Print boot volume reserved logical sector count
    ;--------------------------------------------------------------------------
    mov       esi, dword str_reserved_count
    mov       dx, word [cs:(boot_data+BootData.ReservedCount)]
    call      log_hex

    ;--------------------------------------------------------------------------
    ; Print boot volume table count
    ;--------------------------------------------------------------------------
    mov       esi, dword str_table_count
    xor       dx, dx
    mov       dl, byte [cs:(boot_data+BootData.TableCount)]
    call      log_hex

    ;--------------------------------------------------------------------------
    ; Print boot volume table logical sector count
    ;--------------------------------------------------------------------------
    mov       esi, dword str_table_sector_count
    mov       dx, word [cs:(boot_data+BootData.SectorsPerTable)]
    call      log_hex

    ;--------------------------------------------------------------------------
    ; Print boot volume cluster count
    ;--------------------------------------------------------------------------
    mov       esi, dword str_cluster_count
    mov       edx, dword [cs:(boot_data+BootData.ClusterCount)]
    call      log_hex

    ;--------------------------------------------------------------------------
    ; Print boot volume cluster logical sector count
    ;--------------------------------------------------------------------------
    mov       esi, dword str_cluster_sector_count
    xor       dx, dx
    mov       dl, byte [cs:(boot_data+BootData.SectorsPerCluster)]
    call      log_hex

    ;--------------------------------------------------------------------------
    ; Print boot volume maximum root count entries
    ;--------------------------------------------------------------------------
    mov       dx, word [cs:(boot_data+BootData.RootEntryCount)]
    or        dx, dx
    jz        .print_root_cluster
    mov       esi, dword str_root_count
    call      log_hex
    jmp       .print_root_size

  .print_root_cluster:

    ;--------------------------------------------------------------------------
    ; Print boot volume root directory cluster
    ;--------------------------------------------------------------------------
    mov       esi, dword str_root_cluster
    mov       edx, dword [cs:(boot_data+BootData.RootDirectoryCluster)]
    call      log_hex

  .print_root_size:

    ;--------------------------------------------------------------------------
    ; Print boot volume root directory size
    ;--------------------------------------------------------------------------
    mov       esi, dword str_root_size
    mov       edx, dword [cs:(boot_data+BootData.RootDirectoryCount)]
    call      log_hex

    ret

%endif

;==============================================================================
; Read character from the console
;
; @retval AH Key scan code
; @retval AL ASCII character
;==============================================================================

read_char:

    mov       ah, 0
    int       0x16
    ret

;==============================================================================
; Write a character to the console
;
; @param AL The character to print
;==============================================================================

print_char:

    ;--------------------------------------------------------------------------
    ; Check this is not null character
    ;--------------------------------------------------------------------------
    or        al, al
    jnz       .print_char

    ret

  .print_char:

    ;--------------------------------------------------------------------------
    ; Save registers that will be clobbered
    ;--------------------------------------------------------------------------
    push      eax
    push      ebx

    ;--------------------------------------------------------------------------
    ; Print the character
    ;--------------------------------------------------------------------------
    mov       ah, 0x0E
    mov       bx, word 0x0001
    int       0x10

    ;--------------------------------------------------------------------------
    ; Restore saved registers
    ;--------------------------------------------------------------------------
    pop       ebx
    pop       eax

    ret

;==============================================================================
; Write a hex value to the console.
;
; @param EDX The hex value to print
;==============================================================================

%ifdef DEBUG

print_hex:

    ;--------------------------------------------------------------------------
    ; Save registers that will be clobbered
    ;--------------------------------------------------------------------------
    push      eax
    push      ecx
    push      edx

    ;--------------------------------------------------------------------------
    ; Swap byte order to make output easier
    ;--------------------------------------------------------------------------
    bswap     edx

    ;--------------------------------------------------------------------------
    ; Print all four bytes of input
    ;--------------------------------------------------------------------------
    mov       ecx, dword 4

  .print_hex_byte:

    ;--------------------------------------------------------------------------
    ; Get the next most significant hex byte
    ;--------------------------------------------------------------------------
    mov       al, dl
    ;--------------------------------------------------------------------------
    ; Print the high nibble of the hex byte
    ;--------------------------------------------------------------------------
    shr       al, byte 4
    and       al, byte 0xF
    cmp       al, byte 10
    jb        .print_decimal_high_nibble

    ;--------------------------------------------------------------------------
    ; Print high nibble character A-F
    ;--------------------------------------------------------------------------
    add       al, ('A'-10)
    call      print_char
    jmp       .print_low_nibble

  .print_decimal_high_nibble:

    ;--------------------------------------------------------------------------
    ; Print high nibble character 0-9
    ;--------------------------------------------------------------------------
    add       al, '0'
    call      print_char

  .print_low_nibble:

    ;--------------------------------------------------------------------------
    ; Print the low nibble of the hex byte
    ;--------------------------------------------------------------------------
    mov       al, dl
    and       al, 0xF
    cmp       al, 10
    jb        .print_decimal_low_nibble

    ;--------------------------------------------------------------------------
    ; Print low nibble character A-F
    ;--------------------------------------------------------------------------
    add       al, ('A'-10)
    call      print_char
    jmp       .print_next_hex_byte

  .print_decimal_low_nibble:

    ;--------------------------------------------------------------------------
    ; Print low nibble character 0-9
    ;--------------------------------------------------------------------------
    add       al, '0'
    call      print_char

  .print_next_hex_byte:

    ;--------------------------------------------------------------------------
    ; Advance the input to the next hex byte
    ;--------------------------------------------------------------------------
    shr       edx, 8
    ;--------------------------------------------------------------------------
    ; Decrement the counter and continue if needed
    ;--------------------------------------------------------------------------
    dec       ecx
    jnz       .print_hex_byte

    ;--------------------------------------------------------------------------
    ; Restore saved registers
    ;--------------------------------------------------------------------------
    pop       edx
    pop       ecx
    pop       eax

    ret

%endif

;==============================================================================
; Write a string to the console.
;
; @param ESI The message to print
;==============================================================================

print_string:

    ;--------------------------------------------------------------------------
    ; Save registers that will be clobbered
    ;--------------------------------------------------------------------------
    push      esi
    push      eax

    ;--------------------------------------------------------------------------
    ; Setup string pointer
    ;--------------------------------------------------------------------------
    mov       eax, esi
    shr       eax, 4
    mov       ds, ax
    and       esi, dword 0xF

  .print_string_continue:

    ;--------------------------------------------------------------------------
    ; Get character from string
    ;--------------------------------------------------------------------------
    lodsb
    ;--------------------------------------------------------------------------
    ; Check if null terminator
    ;--------------------------------------------------------------------------
    or        al, al
    jz        .print_string_finish
    ;--------------------------------------------------------------------------
    ; Print character
    ;--------------------------------------------------------------------------
    call      print_char
    jmp       .print_string_continue

  .print_string_finish:

    ;--------------------------------------------------------------------------
    ; Restore clobbered registers
    ;--------------------------------------------------------------------------
    pop       eax
    pop       esi

    ret

;==============================================================================
; Log a prefixed string and hex value to the console.
;
; @param ESI The message to print
; @param EDX The hex to print
;==============================================================================

%ifdef DEBUG

log_hex:

    call      print_string
    call      print_hex
    mov       esi, dword str_new_line
    call      print_string

    ret

%endif

;==============================================================================
; Read physical sectors from the boot device
;
; @param EBX The LBA of the physical sector to start reading
; @param ECX The physical number of sectors to read
; @param EDI The destination address
;==============================================================================

read_sectors:

    ;--------------------------------------------------------------------------
    ; Save registers so they are not clobbered
    ;--------------------------------------------------------------------------
    push      edi
    push      esi
    push      eax
    push      ebx
    push      ecx
    push      edx

    ;--------------------------------------------------------------------------
    ; Check if the interrupt 0x13 LBA extensions are present
    ;--------------------------------------------------------------------------
    mov       dl, byte [cs:(boot_info+BootInfo.BootFlags)]
    and       dl, byte cFeatureExtRW
    jz        .old_read_sectors

  .read_sectors_loop:

    ;--------------------------------------------------------------------------
    ; Check if count is valid
    ;--------------------------------------------------------------------------
    or        ecx, ecx
    jz        .read_sectors_finished

    ;--------------------------------------------------------------------------
    ; Get the maximum count of sectors to read this loop cycle
    ;--------------------------------------------------------------------------
    mov       eax, ecx
    cmp       eax, dword 0x0000007F
    jbe       .valid_sector_count
    mov       eax, dword 0x0000007F

  .valid_sector_count:

    sub       ecx, eax

    ;--------------------------------------------------------------------------
    ; Setup the DAP in the temporary read buffer
    ;--------------------------------------------------------------------------
    mov       esi, dword (boot_data+BootData.Buffer)
    mov       word [cs:(boot_data+BootData.Buffer)], word 0x10
    mov       word [cs:(boot_data+BootData.Buffer+2)], ax
    mov       dword [cs:(boot_data+BootData.Buffer+4)], edi
    shl       dword [cs:(boot_data+BootData.Buffer+4)], 12
    mov       word [cs:(boot_data+BootData.Buffer+4)], di
    and       word [cs:(boot_data+BootData.Buffer+4)], word 0xF
    mov       dword [cs:(boot_data+BootData.Buffer+8)], ebx
    mov       dword [cs:(boot_data+BootData.Buffer+12)], dword 0

    ;--------------------------------------------------------------------------
    ; Advance to the next LBA
    ;--------------------------------------------------------------------------
    add       ebx, eax
    ;--------------------------------------------------------------------------
    ; Advance the read buffer to the read address for the next loop cycle
    ;--------------------------------------------------------------------------
    xor       edx, edx
    push      ecx
    mov       ecx, dword [cs:(boot_info+BootInfo.SectorSize)]
    mul       ecx
    pop       ecx
    add       edi, eax

    ;--------------------------------------------------------------------------
    ; Setup segment and offset from address
    ;--------------------------------------------------------------------------
    mov       eax, esi
    shr       eax, 4
    mov       ds, ax
    and       esi, dword 0xF

    ;--------------------------------------------------------------------------
    ; Call extended read sectors
    ;--------------------------------------------------------------------------
    mov       dl, byte [cs:(boot_info+BootInfo.BootDrive)]
    mov       ah, byte 0x42
    int       0x13
    ;--------------------------------------------------------------------------
    ; Check there was no error by CF
    ;--------------------------------------------------------------------------
    jnc       .read_sectors_loop
    ;--------------------------------------------------------------------------
    ; Check the return status is success
    ;--------------------------------------------------------------------------
    or        ah, ah
    jz        .read_sectors_loop

    ;--------------------------------------------------------------------------
    ; Read failed so boot failed
    ;--------------------------------------------------------------------------
    mov       esi, dword str_read_failed
    jmp       boot_fail

  .old_read_sectors:

    ;--------------------------------------------------------------------------
    ; TODO: Read sectors using old CHS method
    ;--------------------------------------------------------------------------
    mov       esi, dword str_read_failed
    jmp       boot_fail

  .read_sectors_finished:

    ;--------------------------------------------------------------------------
    ; Restore saved registers
    ;--------------------------------------------------------------------------
    pop       edx
    pop       ecx
    pop       ebx
    pop       eax
    pop       esi
    pop       edi

    ret

;==============================================================================
; Read logical sectors from the boot volume
;
; @param EBX The LBA of the logical sector to start reading
; @param ECX The logical number of sectors to read
; @param EDI The destination address
;==============================================================================

read_logical_sectors:

    ;--------------------------------------------------------------------------
    ; Save registers that will be clobbered
    ;--------------------------------------------------------------------------
    push      eax
    push      ebx
    push      ecx
    push      edx

    ;--------------------------------------------------------------------------
    ; Convert logical count to physical count
    ;--------------------------------------------------------------------------
    xor       edx, edx
    mov       eax, ecx
    xor       ecx, ecx
    mov       cx, word [cs:(boot_info+BootInfo.SectorSize)]
    add       eax, ecx
    dec       eax
    div       ecx
    mov       ecx, eax

    ;--------------------------------------------------------------------------
    ; Convert logical LBA to physical LBA and byte offset
    ;--------------------------------------------------------------------------
    xor       edx, edx
    mov       eax, ebx
    xor       ebx, ebx
    mov       bx, word [cs:(boot_data+BootData.SectorSize)]
    mul       ebx
    mov       bx, word [cs:(boot_info+BootInfo.SectorSize)]
    div       ebx
    mov       ebx, eax
    add       ebx, dword [cs:(boot_info+BootInfo.VolumeStart)]

    ;--------------------------------------------------------------------------
    ; Read physical sectors
    ;--------------------------------------------------------------------------
    call      read_sectors

    ;--------------------------------------------------------------------------
    ; Check if the logical sectors need relocated by the byte offset
    ;--------------------------------------------------------------------------
    or        edx, edx
    jz        .read_logical_sectors_finished

    ;--------------------------------------------------------------------------
    ; Relocate the logical sectors by the byte offset
    ;--------------------------------------------------------------------------
    push      edi
    push      esi
    mov       ebx, dword [cs:(boot_data+BootData.SectorSize)]
    mov       eax, edi
    shr       eax, 4
    mov       es, ax
    mov       esi, edi
    and       edi, dword 0xF
    add       esi, edx
    mov       eax, esi
    and       esi, dword 0xF
    shr       eax, 4
    mov       ds, ax
    mov       eax, ecx
    xor       edx, edx
    mul       ebx
    mov       ecx, eax
    a32 rep movsb
    pop       esi
    pop       edi

  .read_logical_sectors_finished:

    ;--------------------------------------------------------------------------
    ; Restore saved registers
    ;--------------------------------------------------------------------------
    pop       edx
    pop       ecx
    pop       ebx
    pop       eax

    ret

;==============================================================================
; Check cluster is valid
;
; @param EBX A cluster
; @retval EBX The given valid cluster or zero
;==============================================================================

check_valid_cluster:

    ;--------------------------------------------------------------------------
    ; Clusters zero and one are reserved, so are invalid
    ;--------------------------------------------------------------------------
    cmp       ebx, dword 2
    jb        .invalid_cluster

    ;--------------------------------------------------------------------------
    ; Clusters above bv_cluster_count are invalid
    ;--------------------------------------------------------------------------
    cmp       ebx, dword [cs:(boot_data+BootData.ClusterCount)]
    jbe       .valid_cluster

  .invalid_cluster:

    ;--------------------------------------------------------------------------
    ; Invalid cluster so set EBX to zero
    ;--------------------------------------------------------------------------
    xor       ebx, ebx

  .valid_cluster:

    ret

;==============================================================================
; Get next cluster
;
; @param EBX A cluster
; @retval EBX The next cluster or zero if there is no next cluster
;==============================================================================

get_next_cluster:

    ;--------------------------------------------------------------------------
    ; Check cluster is valid
    ;--------------------------------------------------------------------------
    call      check_valid_cluster
    or        ebx, ebx
    jnz       .valid_cluster

  .valid_cluster:

    ;--------------------------------------------------------------------------
    ; Save the registers that will be clobbered
    ;--------------------------------------------------------------------------
    push      esi
    push      eax
    push      ecx
    push      edx

    ;--------------------------------------------------------------------------
    ; Get the cluster table address
    ;--------------------------------------------------------------------------
    mov       esi, dword [cs:(boot_data+BootData.Table)]
    mov       eax, esi
    shr       eax, 4
    mov       ds, ax
    and       esi, dword 0xF

    ;--------------------------------------------------------------------------
    ; Check bit width of a cluster for this volume
    ;--------------------------------------------------------------------------
    cmp       dword [cs:(boot_data+BootData.ClusterCount)], dword 0xFFF6
    jae       .check_fat32
    cmp       dword [cs:(boot_data+BootData.ClusterCount)], dword 0xFF6
    jae       .check_fat16

    ;--------------------------------------------------------------------------
    ; Get the next cluster for FAT12
    ;--------------------------------------------------------------------------
    xor       edx, edx
    mov       eax, ebx
    shr       eax, 1
    mov       ecx, dword 3
    mul       ecx
    add       esi, eax
    and       ebx, dword 1
    jnz       .check_fat12

    mov       ebx, dword [ds:esi]
    and       ebx, 0xFFF
    jmp       .get_next_cluster_done

  .check_fat12:

    mov       ebx, dword [ds:esi]
    shr       ebx, 12
    and       ebx, 0xFFF
    jmp       .get_next_cluster_done

  .check_fat16:

    ;--------------------------------------------------------------------------
    ; Get the next cluster for FAT16
    ;--------------------------------------------------------------------------
    shl       ebx, 1
    add       esi, ebx
    mov       ebx, dword [ds:esi]
    and       ebx, 0xFFFF
    jmp       .get_next_cluster_done

  .check_fat32:
    
    ;--------------------------------------------------------------------------
    ; Get the next cluster for FAT32
    ;--------------------------------------------------------------------------
    shl       ebx, 2
    add       esi, ebx
    mov       ebx, dword [ds:esi]
    and       ebx, 0xFFFFFFF

  .get_next_cluster_done:

    ;--------------------------------------------------------------------------
    ; Restore the registers that were clobbered
    ;--------------------------------------------------------------------------
    pop       edx
    pop       ecx
    pop       eax
    pop       esi

    ;--------------------------------------------------------------------------
    ; Check cluster is valid
    ;--------------------------------------------------------------------------
    call      check_valid_cluster

    ret

;==============================================================================
; Load cluster from boot volume
;
; @param EBX The cluster to load
; @param EDI The memory address to load the cluster
; @retval ECX The size in bytes of the loaded cluster
;==============================================================================

load_cluster:

    ;--------------------------------------------------------------------------
    ; Check cluster is valid
    ;--------------------------------------------------------------------------
    call      check_valid_cluster
    or        ebx, ebx
    jnz       .valid_cluster
    mov       esi, dword str_invalid_cluster
    jmp       boot_fail

  .valid_cluster:

    ;--------------------------------------------------------------------------
    ; Save the registers that will be clobbered
    ;--------------------------------------------------------------------------
    push      eax
    push      ebx
    push      edx

%ifdef DEBUG

    mov       edx, ebx
    call      print_hex
    mov       esi, dword str_new_line
    call      print_string

%endif

    ;--------------------------------------------------------------------------
    ; Read the cluster
    ;--------------------------------------------------------------------------
    sub       ebx, dword 2
    mov       eax, ebx
    xor       edx, edx
    xor       ecx, ecx
    mov       cl, byte [cs:(boot_data+BootData.SectorsPerCluster)]
    mul       ecx
    mov       ebx, eax
    add       ebx, dword [cs:(boot_data+BootData.Start)]
    call      read_logical_sectors

    ;--------------------------------------------------------------------------
    ; Get the size of the cluster
    ;--------------------------------------------------------------------------
    xor       eax, eax
    mov       al, byte [cs:(boot_data+BootData.ClusterCount)]
    mul       word [cs:(boot_data+BootData.SectorSize)]
    shl       edx, 16
    mov       dx, ax
    mov       ecx, edx

    ;--------------------------------------------------------------------------
    ; Restore the registers that were clobbered
    ;--------------------------------------------------------------------------
    pop       edx
    pop       ebx
    pop       eax

    ret

;==============================================================================
; Load cluster(s) from boot volume
;
; @param EBX The starting cluster
; @param EDI The memory address to load the cluster(s)
; @retval ECX The size in bytes of the loaded cluster(s)
;==============================================================================

load_clusters:

    ;--------------------------------------------------------------------------
    ; Check cluster is valid
    ;--------------------------------------------------------------------------
    call      check_valid_cluster
    or        ebx, ebx
    jnz       .valid_cluster
    mov       esi, dword str_invalid_cluster
    jmp       boot_fail

  .valid_cluster:

    ;--------------------------------------------------------------------------
    ; Save the registers that will be clobbered
    ;--------------------------------------------------------------------------
    push      edi
    push      esi
    push      eax
    push      ebx
    push      edx
    xor       ecx, ecx

  .load_next_cluster:

    ;--------------------------------------------------------------------------
    ; Load each cluster in the cluster chain
    ;--------------------------------------------------------------------------
    push      ecx
    call      load_cluster
    mov       ecx, dword 0x200
    add       edi, ecx
    pop       eax
    add       ecx, eax
    call      get_next_cluster
    or        ebx, ebx
    jnz       .load_next_cluster

    ;--------------------------------------------------------------------------
    ; Restore the registers that were clobbered
    ;--------------------------------------------------------------------------
    pop       edx
    pop       ebx
    pop       eax
    pop       esi
    pop       edi

    ret

;==============================================================================
; Find the boot loader starting cluster
;
; @param ESI The 11 byte name of the file (padded with spaces if needed)
; @retval EBX The boot loader starting cluster
;==============================================================================

find_boot_cluster:

    ;--------------------------------------------------------------------------
    ; Save registers that will be clobbered
    ;--------------------------------------------------------------------------
    push      edi
    push      esi
    push      eax
    push      ecx

    ;--------------------------------------------------------------------------
    ; Set EBX to zero in case the starting cluster is not found
    ;--------------------------------------------------------------------------
    xor       ebx, ebx

    ;--------------------------------------------------------------------------
    ; Setup the search for the next stage boot loader cluster from boot volume
    ;--------------------------------------------------------------------------
    mov       ecx, dword [cs:(boot_data+BootData.RootDirectorySize)]
    shr       ecx, 4
    or        ecx, ecx
    jz        .find_boot_cluster_done
    mov       edi, dword [cs:(boot_data+BootData.RootDirectory)]
    push      ds
    push      es
    mov       eax, edi
    shr       eax, 4
    mov       es, ax
    and       edi, dword 0xF
    mov       eax, esi
    shr       eax, 4
    mov       ds, ax
    and       esi, dword 0xF

    ;--------------------------------------------------------------------------
    ; Search for the next stage boot loader cluster
    ;--------------------------------------------------------------------------

  .find_next:

    push      edi
    push      esi
    push      ecx
    mov       ecx, dword 11

  .strcmp:

    cmpsb
    jnz       .not_match
    dec       ecx
    jnz       .strcmp
    jmp       .boot_cluster_found

  .not_match:

    pop       ecx
    pop       esi
    pop       edi
    add       edi, dword 0x20
    dec       ecx
    jnz       .find_next
    jmp       .find_boot_cluster_done

    ;--------------------------------------------------------------------------
    ; Boot loader cluster found
    ;--------------------------------------------------------------------------

  .boot_cluster_found:

    pop       ecx
    pop       esi
    pop       edi
    
    ;--------------------------------------------------------------------------
    ; Get the cluster from the directory entry in EDI
    ;--------------------------------------------------------------------------
    mov       bx, word [es:(edi+cVolumeEntryHighCluster)]
    shl       ebx, 16
    mov       bx, word [es:(edi+cVolumeEntryLowCluster)]
    and       ebx, 0xFFFFFFF

    ;--------------------------------------------------------------------------
    ; Restore clobbered registers
    ;--------------------------------------------------------------------------
    pop       es
    pop       ds

    ;--------------------------------------------------------------------------
    ; Check the bit width of a cluster for this volume
    ;--------------------------------------------------------------------------
    cmp       dword [cs:(boot_data+BootData.ClusterCount)], dword 0xFFF6
    jae       .find_boot_cluster_done
    and       ebx, dword 0xFFFF
    cmp       dword [cs:(boot_data+BootData.ClusterCount)], dword 0xFF6
    jae       .find_boot_cluster_done
    and       ebx, dword 0xFFF

  .find_boot_cluster_done:

    ;--------------------------------------------------------------------------
    ; Restore clobbered registers and return when no matches found
    ;--------------------------------------------------------------------------
    pop       ecx
    pop       eax
    pop       esi
    pop       edi

    ret

;==============================================================================
; Loader messages
;------------------------------------------------------------------------------

    ;--------------------------------------------------------------------------
    ; Restart message
    ;--------------------------------------------------------------------------
    str_restart:   db  'Press key to restart', 0
    ;--------------------------------------------------------------------------
    ; New line message
    ;--------------------------------------------------------------------------
    str_new_line:  db  0x0D, 0x0A, 0

;==============================================================================
; Loader error messages
;------------------------------------------------------------------------------

    ;--------------------------------------------------------------------------
    ; Invalid LBA
    ;--------------------------------------------------------------------------
    str_invalid_cluster:  db  'Invalid cluster', 0
    ;--------------------------------------------------------------------------
    ; Read failed
    ;--------------------------------------------------------------------------
    str_read_failed:      db  'Read failed', 0
    ;--------------------------------------------------------------------------
    ; Not found
    ;--------------------------------------------------------------------------
    str_not_found:        db  'Not found', 0
    ;--------------------------------------------------------------------------
    ; Failure message
    ;--------------------------------------------------------------------------
    str_failure:          db  'Unknown failure', 0

;==============================================================================
; Loader debug messages
;------------------------------------------------------------------------------

%ifdef DEBUG

    ;--------------------------------------------------------------------------
    ; Logical sector start message
    ;--------------------------------------------------------------------------
    str_physical_start:         db  'Physical start: ', 0
    ;--------------------------------------------------------------------------
    ; Physical sector count message
    ;--------------------------------------------------------------------------
    str_physical_sector_count:  db  'Physical count: ', 0
    ;--------------------------------------------------------------------------
    ; Logical sector count message
    ;--------------------------------------------------------------------------
    str_physical_sector_size    db  'Physical size: ', 0

    ;--------------------------------------------------------------------------
    ; Logical sector start message
    ;--------------------------------------------------------------------------
    str_logical_start:          db  'Logical start: ', 0
    ;--------------------------------------------------------------------------
    ; Logical sector count message
    ;--------------------------------------------------------------------------
    str_logical_sector_count    db  'Logical count: ', 0
    ;--------------------------------------------------------------------------
    ; Logical sector size message
    ;--------------------------------------------------------------------------
    str_logical_sector_size:    db  'Logical size: ', 0

    ;--------------------------------------------------------------------------
    ; Logical reserved sectors message
    ;--------------------------------------------------------------------------
    str_reserved_count:         db  'Logical reserved: ', 0
    ;--------------------------------------------------------------------------
    ; Cluster table count message
    ;--------------------------------------------------------------------------
    str_table_count:            db  'Cluster tables: ', 0
    ;--------------------------------------------------------------------------
    ; Logical cluster table sector count message
    ;--------------------------------------------------------------------------
    str_table_sector_count:     db  'Cluster table sectors: ', 0

    ;--------------------------------------------------------------------------
    ; Cluster count message
    ;--------------------------------------------------------------------------
    str_cluster_count           db  'Clusters: ', 0
    ;--------------------------------------------------------------------------
    ; Cluster logical sector count message
    ;--------------------------------------------------------------------------
    str_cluster_sector_count:   db  'Cluster sectors: ', 0

    ;--------------------------------------------------------------------------
    ; Root entry count message
    ;--------------------------------------------------------------------------
    str_root_count:             db  'Root count: ', 0
    ;--------------------------------------------------------------------------
    ; Root cluster message
    ;--------------------------------------------------------------------------
    str_root_cluster            db  'Root cluster: ', 0
    ;--------------------------------------------------------------------------
    ; Root size message
    ;--------------------------------------------------------------------------
    str_root_size               db  'Root size: ', 0
    ;--------------------------------------------------------------------------
    ; Boot cluster message
    ;--------------------------------------------------------------------------
    str_boot_cluster            db  'Boot cluster: ', 0

%endif

;==============================================================================
; Loader size in bytes
;==============================================================================

    ;--------------------------------------------------------------------------
    ; Loader size in bytes
    ;--------------------------------------------------------------------------
    boot_size: dd (4+($-$$))

;==============================================================================
; Temporary read buffer and padding
;==============================================================================

    ;--------------------------------------------------------------------------
    ; 16-byte aligned
    ;--------------------------------------------------------------------------

%if (($-$$)&15) != 0

    times     (16-(($-$$)&15))  db  0

%endif

    ;--------------------------------------------------------------------------
    ; Boot data structure will be used here during execution
    ;--------------------------------------------------------------------------

  boot_data:

    ;--------------------------------------------------------------------------
    ; This pads the loader with zeros to force multiple of default sector size
    ;--------------------------------------------------------------------------
    times     ((((($-$$)+(cDefaultSectorSize-1))/cDefaultSectorSize)*cDefaultSectorSize)-($-$$))  db  0

