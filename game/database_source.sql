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

INSERT INTO "dialogue" VALUES('/world_1/intro','pyland_public_announcement','Message to all Pyland Citizens: Click on the blue arrow button or press ''Enter'' to progress through dialogue.','Untranslated','Untranslated','Untranslated','Message t'' all ye Pyland Citizens: Click on t'' blue arrow button or press ''Enter'' t'' progress through dialogue.');
INSERT INTO "dialogue" VALUES('shared','yes','Yes','Untranslated french','Untranslated dutch','Untranslated hindi','Aye!');
INSERT INTO "dialogue" VALUES('shared','no','No','Untranslated french','Untranslated dutch','Untranslated hindi','No!');
INSERT INTO "dialogue" VALUES('/world_1/intro','monty_coming_now','Aaah, I''m coming now! I''ll just be a second.','Untranslated to French.','Untranslated to Dutch.','Untranslated to Hindi.','Yeaarrr, I''m comin'' now! I''ll just be a second!');
INSERT INTO "dialogue" VALUES('/world_1/intro','im_monty_the_snake',' I haven''t seen you around here before. I''m Monty and I''m going to introduce you to Pyland. I think you''re going to like it here!','Untranslated','Untranslated','Untranslated','I haven''t seen you around here before. I''m Monty and I''m goin'' t'' introduce you t'' Pyland. Me thinks you''re going t''enjoy it ''ere!');
INSERT INTO "dialogue" VALUES('/world_1/intro','monty_doesnt_know_name','Oh, how rude of me, I don''t even know your name!','Untranslated','Untranslated','Untranslated','Oh, how rude o'' me, I almost forgot I don''t even know your name!');
INSERT INTO "dialogue" VALUES('/world_1/intro','big_white_box','You see the big grey box below me? That is where you''ll write your code.  It''s called the PyScripter.','Untranslated','Untranslated','Untranslated','Ye spy wit'' ye eye th'' big white box below me? That be just where ye''ll write code. gift it a click ''n replace ''YourName'' wit'', well, ye moniker ''o course! Hit th'' ''Run'' button in th'' bottom-right hand corner when done.');
INSERT INTO "dialogue" VALUES('/world_1/intro','scripting','Click the PyScripter below and replace ''YourName'' with, well, your name of course! Hit the ''Run'' button in the bottom-right hand corner when you''re done.','Untranslated','Untranslated','Untranslated','Giv'' t  click n'' replace ''YourName'' wit'' yer name! Hit t'' ''Run'' button in t'' bottom-right hand corner when yer done.');
INSERT INTO "dialogue" VALUES('/world_1/intro','confirm_player_name','Well hello %player_name%, you are called %player_name% right?','Untranslated','Untranslated','Untranslated','Ahoy %player_name%! Ye be called %player_name% right?');
INSERT INTO "dialogue" VALUES('/world_1/intro','player_name_wrong','Oh sorry! Can you try telling me your name again?','Untranslated','Untranslated','Untranslated','Avast! Apologies for gettin'' it wrong. Give it another go you.');
INSERT INTO "dialogue" VALUES('/world_1/intro','wrote_first_program','Brilliant %player_name%, you have just written a program! That is absolutey spiffing! You are well onto your way to becoming a master of code!','Untranslated','Untranslated','Untranslated','Avast %player_name%! Ye have just written a program! That be absolutey spiffing! Ye be well onto ye way to becoming a master like me.');
INSERT INTO "dialogue" VALUES('/world_1/intro','console_output','Now you see the smaller grey box next to the PyScripter? That is called the PyConsole - everything you print will be shown in there! It will be a really helpful tool as you progress through Pyland!','Untranslated','Untranslated','Untranslated','Now ye spy the wee white box next to the text-box ye just wrote yer program in? That be called the console-output, and everything ye print will be shown in there! Yaaar, it will be a really helpful tool as ye progress through Pyland!');
INSERT INTO "dialogue" VALUES('/world_1/intro','go_enjoy_pyland','Anyway %player_name%, don''t let me impede on your adventure any longer! Just one last piece of advice from me. ','Untranslated','Untranslated','Untranslated','Ahoy %player_name%! Don''t let me impede ye any longerrr! Go and enjoy Pyland.');
INSERT INTO "dialogue" VALUES('/world_1/intro','keys','Use the arrow keys to move around and press ''Enter'' to interact with people and objects.','Untranslated','Untranslated','Untranslated','You can use the arrow keys to move around and press ''Enter'' to interact with people and objects.');
INSERT INTO "dialogue" VALUES('/world_1/level_1/rossumberg','heidi_shout_player_name','Heidi: %player_name%!!!!','Untranslated french','Untranslated dutch','Untranslated hindi','Heidi: Ahoy %player_name%!!!!');
INSERT INTO "dialogue" VALUES('/world_1/level_1/rossumberg','heidi_today_is_big_day','Today is the big day! You did remember right?','Untranslated french','Untranslated dutch','Untranslated hindi','Today be t'' grand day! Ye did remember right?');
INSERT INTO "dialogue" VALUES('/world_1/level_1/rossumberg','heidi_question_player_about_day','Ok, so what are what are we doing today then?','Untranslated french','Untranslated dutch','Untranslated hindi','Ahoy, so what we be doin'' then?');
-- INSERT INTO "dialogue" VALUES('world_1/level_1/rossumberg','','','Untranslated french','Untranslated dutch','Untranslated hindi','');
COMMIT;
