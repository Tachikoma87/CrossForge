CREATE DATABASE crossforgetest WITH OWNER 'crossforge-peon';

DROP TABLE IF EXISTS version;
CREATE TABLE version (
    version_id          BIGSERIAL PRIMARY KEY,
    major               INT,
    minor               INT,
    patch               INT,
    timestamp_created   TIMESTAMP
);