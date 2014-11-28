# This file expects that OBJ contains a list of all of the object files.
# The directory portion of each object file is used to locate the source
# and should not contain any ..'s but rather be relative to the top of the 
# tree.
#
# So for example, py/map.c would have an object file name py/map.o
# The object files will go into the build directory and mantain the same
# directory structure as the source tree. So the final dependency will look
# like this:
#
# build/py/map.o: py/map.c
#
# We set vpath to point to the top of the tree so that the source files
# can be located. By following this scheme, it allows a single build rule
# to be used to compile all .c files.

vpath %.S . $(TOP)
$(BUILD)/%.o: %.S
	$(ECHO) "AS $<"
	$(Q)$(AS) $(AFLAGS) -c -o $@ $<

vpath %.s . $(TOP)
$(BUILD)/%.o: %.s
	$(ECHO) "AS $<"
	$(Q)$(AS) -o $@ $<

define compile_c
$(ECHO) "CC $<"
$(Q)$(CC) $(CFLAGS) -c -MD -o $@ $<
@# The following fixes the dependency file.
@# See http://make.paulandlesley.org/autodep.html for details.
@$(CP) $(@:.o=.d) $(@:.o=.P); \
  $(SED) -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
      -e '/^$$/ d' -e 's/$$/ :/' < $(@:.o=.d) >> $(@:.o=.P); \
  $(RM) -f $(@:.o=.d)
endef

vpath %.c . $(TOP)
$(BUILD)/%.o: %.c
	$(call compile_c)

$(BUILD)/%.pp: %.c
	$(ECHO) "PreProcess $<"
	$(Q)$(CC) $(CFLAGS) -E -Wp,-C,-dD,-dI -o $@ $<
