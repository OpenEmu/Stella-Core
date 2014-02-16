MODULE := src/gui

MODULE_OBJS := \
	src/gui/AboutDialog.o \
	src/gui/AudioDialog.o \
	src/gui/BrowserDialog.o \
	src/gui/ComboDialog.o \
	src/gui/CommandDialog.o \
	src/gui/CommandMenu.o \
	src/gui/ContextMenu.o \
	src/gui/DialogContainer.o \
	src/gui/Dialog.o \
	src/gui/EditableWidget.o \
	src/gui/EditTextWidget.o \
	src/gui/EventMappingWidget.o \
	src/gui/FileListWidget.o \
	src/gui/ConfigPathDialog.o \
	src/gui/SnapshotDialog.o \
	src/gui/Font.o \
	src/gui/GameInfoDialog.o \
	src/gui/GameList.o \
	src/gui/GlobalPropsDialog.o \
	src/gui/HelpDialog.o \
	src/gui/InputDialog.o \
	src/gui/InputTextDialog.o \
	src/gui/Launcher.o \
	src/gui/LauncherDialog.o \
	src/gui/LauncherFilterDialog.o \
	src/gui/LoggerDialog.o \
	src/gui/ListWidget.o \
	src/gui/Menu.o \
	src/gui/MessageBox.o \
	src/gui/OptionsDialog.o \
	src/gui/PopUpWidget.o \
	src/gui/ProgressDialog.o \
	src/gui/RomAuditDialog.o \
	src/gui/RomInfoWidget.o \
	src/gui/ScrollBarWidget.o \
	src/gui/CheckListWidget.o \
	src/gui/StringListWidget.o \
	src/gui/TabWidget.o \
	src/gui/UIDialog.o \
	src/gui/VideoDialog.o \
	src/gui/Widget.o

MODULE_DIRS += \
	src/gui

# Include common rules 
include $(srcdir)/common.rules
