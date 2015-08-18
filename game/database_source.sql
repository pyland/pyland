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
COMMIT;
