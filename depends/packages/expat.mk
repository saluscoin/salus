package=expat
$(package)_version=2.2.1
$(package)_download_path=https://src.fedoraproject.org/repo/pkgs/expat/expat-2.1.1.tar.bz2/7380a64a8e3a9d66a9887b01d0d7ea81/
$(package)_file_name=$(package)-$($(package)_version).tar.bz2
$(package)_sha256_hash=1868cadae4c82a018e361e2b2091de103cd820aaacb0d6cfa49bd2cd83978885

define $(package)_set_vars
$(package)_config_opts=--disable-static
endef

define $(package)_config_cmds
  $($(package)_autoconf)
endef

define $(package)_build_cmds
  $(MAKE)
endef

define $(package)_stage_cmds
  $(MAKE) DESTDIR=$($(package)_staging_dir) install
endef
