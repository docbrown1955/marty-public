.PHONY: marty dep install jsonparser csl grafed clean

BLUE=\033[1;34m
NC=\033[0m # No Color
BOLD=\033[1m
NORMAL=\033[0m
GREEN=\033[0;32m

all:
	@echo "$(GREEN)$(BOLD)This may take a while, why don't you grab a cup of coffee ? :)$(NC)$(NORMAL)"
	@sleep 2 
	$(MAKE) dep
	$(MAKE) csl
	$(MAKE) jsonparser
	$(MAKE) grafed
	$(MAKE) marty
	@echo "$(GREEN)$(BOLD)Compilation finished, you may now type $(BLUE)\n $$ make install$(NC)$(NORMAL)"
dep:
	cd dependencies&& $(MAKE) dep
csl:
	cd csl&&          $(MAKE) all
jsonparser:
	cd jsonparser&&   $(MAKE) all
grafed:
	cd grafed&&       $(MAKE) all
marty:
	cd marty/models&& $(MAKE) all
	cd marty&&        $(MAKE) all

install:
	cd marty&&        $(MAKE) install
	cd csl&&          $(MAKE) install
	cd grafed&&       $(MAKE) install
	cd jsonparser&&   $(MAKE) install
	cd dependencies&& $(MAKE) install
	@echo "$(GREEN)$(BOLD)The installation is finished, you may now watch the trilogy 'Back to the future' "
	@echo "to honor the McFly family !"
	@echo "(or start using the code, as you want)$(NC)$(NORMAL)"

clean:
	cd marty&&        $(MAKE) clean&
	cd csl&&          $(MAKE) clean&
	cd grafed&&       $(MAKE) clean&
	cd jsonparser&&   $(MAKE) clean&
	cd dependencies&& $(MAKE) clean
