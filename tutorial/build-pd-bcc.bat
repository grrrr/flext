@echo --- Building flext tutorial examples with BorlandC++ ---

@cd simple1
@make -f ..\makefile.pd-bcc NAME=simple1 SETUPFUNCTION=simple1_setup
@cd ..

@cd simple2
@make -f ..\makefile.pd-bcc NAME=simple2 SETUPFUNCTION=simple2_setup
@cd ..

@cd simple3
@make -f ..\makefile.pd-bcc NAME=simple3 SETUPFUNCTION=simple3_setup
@cd ..

@cd adv1
@make -f ..\makefile.pd-bcc NAME=adv1 SETUPFUNCTION=adv1_setup
@cd ..

@cd signal1
@make -f ..\makefile.pd-bcc NAME=signal1~ SETUPFUNCTION=signal1_tilde_setup
@cd ..

@cd signal2
@make -f ..\makefile.pd-bcc NAME=signal2~ SETUPFUNCTION=signal2_tilde_setup
@cd ..


