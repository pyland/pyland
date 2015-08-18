PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;
CREATE TABLE "dialogue" (
    "level" TEXT NOT NULL,
    "identifier" TEXT NOT NULL,
    "english" TEXT,
    "french" TEXT,
    "dutch" TEXT,
    "hindi" TEXT,
    "pyrate" TEXT
);
INSERT INTO "dialogue" VALUES('world_1/intro','monty_coming_now','Aaah, I''m coming now! I''ll just be a second','Untranslated to French.','Untranslated to Dutch.','Untranslated to Hindi.','Yeaarrr, I''m comin'' now! I''ll just be a second!');
INSERT INTO "dialogue" VALUES('world_1/intro','im_monty_the_snake',' I haven''t seen you around here before. I''m Monty and I''m going to introduce you to Pyland. I''m the 4th wall breaking snake so I can help you in ways some of the lesser able inhabitants here can''t.','Untranslated','Untranslated','Untranslated','I haven''t seen you around here before. I''m Monty and I''m goin'' t'' introduce you t'' Pyland. I''m t'' 4th wall breakin'' snake so I can help you in ways some o'' t'' lesser able inhabitants here can''t.');
INSERT INTO "dialogue" VALUES('world_1/intro','monty_doesnt_know_name','Oh, how rude of me, I almost forgot I don''t even know your name!','Untranslated','Untranslated','Untranslated','Oh, how rude o'' me, I almost forgot I don''t even know your name!');
INSERT INTO "dialogue" VALUES('world_1/intro','monty_text_editor','You''re going to have to tell me your name through the PyScripter. PyScripter is a bit of a cheesy name I know, but I didn''t pick it the designers of this game did!','Untranslated','Untranslated','Untranslated','You''re goin'' t'' have t'' tell me your name through t'' PyScripter,tis a bit o'' a cheesy name I know, but I didn''t pick it t'' designers o'' this game did!');
INSERT INTO "dialogue" VALUES('world_1/intro','big_white_box','So you see the big white box below me? That is just what you''ll need. Give it a click and replace ''YourName'' with, well, your name of course! Hit the ''Run'' button in the bottom-right hand corner when done.','Untranslated','Untranslated','Untranslated','So ye spy wit'' ye eye th'' big white box below me? That be just what ye''ll need. gift it a click ''n replace ''YourName'' wit'', well, ye moniker ''o course! Hit th'' ''Run'' button in th'' bottom-right hand corner when done.');
INSERT INTO "dialogue" VALUES('world_1/intro','confirm_player_name','Well hello %player_name%, you are called %player_name% right?','Untranslated','Untranslated','Untranslated','Ahoy %player_name%! Ye be called %player_name% right?');
INSERT INTO "dialogue" VALUES('world_1/intro','wrote_first_program','Brilliant %player_name%, you have just written a program! That is absolutey spiffing! You are well onto your way to becoming a master like me!','Untranslated','Untranslated','Untranslated','Avast %player_name%! Ye have just written a program! That be absolutey spiffing! Ye be well onto ye way to becoming a master like me.');
INSERT INTO "dialogue" VALUES('world_1/intro','console_output','Now you see the little white box next to the text-box you just wrote your program in? That is called the console-output, and everything you ''print'' will be shown in there! It will be a really helpful tool as you progress through Pyland!','Untranslated','Untranslated','Untranslated','Now ye spy the wee white box next to the text-box ye  just wrote yer program in? That be called the console-output, and everything ye ''print'' will be shown in there! Yaaar, it will be a really helpful tool as ye progress through Pyland!');
INSERT INTO "dialogue" VALUES('world_1/intro','go_enjoy_pyland','Anyway %player_name%, don''t let me impede you any longer! Go and enjoy Pyland.','Untranslated','Untranslated','Untranslated','Ahoy %player_name%! Don''t let me impede ye any longerrr! Go and enjoy Pyland.');
COMMIT;
