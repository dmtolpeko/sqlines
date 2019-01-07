CREATE OR REPLACE FUNCTION test_returning(
          p1 OUT VARCHAR(3), p2 OUT VARCHAR(3) ) AS $$

BEGIN
   p1 := '0';p2 := '1';
END; 
$$ LANGUAGE plpgsql;
