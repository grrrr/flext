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

@cd adv2
@make -f ..\makefile.pd-bcc NAME=adv2 SETUPFUNCTION=adv2_setup
@cd ..

@cd adv3
@make -f ..\makefile.pd-bcc NAME=adv3 SETUPFUNCTION=adv3_setup
@cd ..

@cd attr1
@make -f ..\makefile.pd-bcc NAME=attr1 SETUPFUNCTION=attr1_setup
@cd ..

@cd attr2
@make -f ..\makefile.pd-bcc NAME=attr2 SETUPFUNCTION=attr2_setup
@cd ..

@cd attr3
@make -f ..\makefile.pd-bcc NAME=attr3 SETUPFUNCTION=attr3_setup
@cd ..

@cd timer1
@make -f ..\makefile.pd-bcc NAME=timer1 SETUPFUNCTION=timer1_setup
@cd ..

@cd signal1
@make -f ..\makefile.pd-bcc NAME=signal1~ SETUPFUNCTION=signal1_tilde_setup
@cd ..

@cd signal2
@make -f ..\makefile.pd-bcc NAME=signal2~ SETUPFUNCTION=signal2_tilde_setup
@cd ..

@cd lib1
@make -f ..\makefile.pd-bcc NAME=lib1 SETUPFUNCTION=lib1_setup
@cd ..

@cd bind1
@make -f ..\makefile.pd-bcc NAME=bind1 SETUPFUNCTION=bind1_setup
@cd ..

@cd buffer1
@make -f ..\makefile.pd-bcc NAME=buffer1 SETUPFUNCTION=buffer1_setup
@cd ..

@rem @cd sndobj1
@rem @make -f ..\makefile.pd-bcc NAME=sndobj1~ SETUPFUNCTION=sndobj1_tilde_setup
@rem @cd ..

@rem @cd stk1
@rem @make -f ..\makefile.pd-bcc NAME=stk1~ SETUPFUNCTION=stk1_tilde_setup
@rem @cd ..

@rem @cd stk2
@rem @make -f ..\makefile.pd-bcc NAME=stk2~ SETUPFUNCTION=stk2_tilde_setup
@rem @cd ..
