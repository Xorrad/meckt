#include "doctest/doctest.hpp"

namespace fs = std::filesystem;

struct TempDir {
    fs::path path;

    TempDir() {
        do {
            path = fs::temp_directory_path() / fs::path("meckt-" + std::to_string(rand()));
        }
        while (fs::exists(path));
        fs::create_directories(path);
    }

    ~TempDir() {
        fs::remove_all(path);
    }
};

TEST_SUITE("[File] ListFiles") {

TEST_CASE("[File] ListFiles: non-existing directory") {
    auto result = File::ListFiles("non_existing_directory", false);
    CHECK(result.empty());
}

TEST_CASE("[File] ListFiles: empty directory") {
    TempDir tmp;

    auto result = File::ListFiles(tmp.path.string(), false);
    CHECK(result.empty());
}

TEST_CASE("[File] ListFiles") {
    TempDir tmp;

    /////////////////////////
    // Create structure:
    // tmp/
    //   file1.txt
    //   file2.txt
    //   subdir/
    //       nested.txt
    fs::path file1 = tmp.path / "file1.txt";
    fs::path file2 = tmp.path / "file2.txt";
    fs::path subdir = tmp.path / "subdir";
    fs::path nested = subdir / "nested.txt";

    fs::create_directories(subdir);
    std::ofstream(file1.string());
    std::ofstream(file2.string());
    std::ofstream(nested.string());
    /////////////////////////

    auto result = File::ListFiles(tmp.path.string(), false);
    CHECK(result.size() == 3);
    CHECK(result.count(file1.string()) == 1);
    CHECK(result.count(file2.string()) == 1);
    CHECK(result.count(subdir.string()) == 1);
    CHECK(result.count(nested.string()) == 0);

    result = File::ListFiles(tmp.path.string(), true);
    CHECK(result.size() == 4);
    CHECK(result.count(file1.string()) == 1);
    CHECK(result.count(file2.string()) == 1);
    CHECK(result.count(subdir.string()) == 1);
    CHECK(result.count(nested.string()) == 1);
}

}

TEST_SUITE("[File] ReadString") {

TEST_CASE("[File] ReadString: full file content") {
    TempDir tmp;
    fs::path file1 = tmp.path / "test.txt";
    fs::path file2 = tmp.path / "large.txt";

    std::string content1 = "Hello\nWorld\n123";
    std::string content2(100000, 'A');

    {
        std::ofstream out(file1);
        out << content1;
    }
    {
        std::ofstream out(file2);
        out << content2;
    }

    std::ifstream in1(file1);
    REQUIRE(in1.is_open());
    CHECK_EQ(File::ReadString(in1), content1);

    std::ifstream in2(file2);
    REQUIRE(in2.is_open());
    CHECK_EQ(File::ReadString(in2), content2);
}

TEST_CASE("[File] ReadString: empty file") {
    TempDir tmp;
    fs::path filePath = tmp.path / "empty.txt";

    {
        std::ofstream out(filePath);
    }

    std::ifstream in(filePath);
    REQUIRE(in.is_open());

    std::string result = File::ReadString(in);
    CHECK(result.empty());
}

TEST_CASE("[File] ReadString: reads large content correctly") {
    TempDir tmp;
    fs::path filePath = tmp.path / "large.txt";

    std::string content(100000, 'A'); // 100k characters
    {
        std::ofstream out(filePath);
        out << content;
    }

    std::ifstream in(filePath);
    REQUIRE(in.is_open());

    std::string result = File::ReadString(in);

    CHECK(result.size() == content.size());
    CHECK(result == content);
}

TEST_CASE("[File] ReadString: invalid stream") {
    std::ifstream in;
    CHECK_THROWS_AS(File::ReadString(in), std::runtime_error);
}

}

TEST_SUITE("[File] ReadCSV") {

TEST_CASE("[File] ReadCSV: non-existing file") {
    CHECK_THROWS_AS(File::ReadCSV("file.csv", ";"), std::runtime_error);
}

TEST_CASE("[File] ReadCSV: empty file") {
    TempDir tmp;
    fs::path filePath = tmp.path / "file.csv";

    {
        std::ofstream out(filePath);
    }

    auto result = File::ReadCSV(filePath.string(), ";");
    REQUIRE(result.size() == 0);
}

TEST_CASE("[File] ReadCSV: simple file") {
    TempDir tmp;
    fs::path filePath = tmp.path / "file.csv";

    {
        std::ofstream out(filePath);
        out << "x\n";
        out << "a;b;c\n";
        out << "1;2;3\n";
    }

    auto result = File::ReadCSV(filePath.string(), ";");
    REQUIRE(result.size() == 3);
    CHECK(result[0] == std::vector<std::string>{"x"});
    CHECK(result[1] == std::vector<std::string>{"a","b","c"});
    CHECK(result[2] == std::vector<std::string>{"1","2","3"});
}

TEST_CASE("[File] ReadCSV: removes comments") {
    TempDir tmp;
    fs::path filePath = tmp.path / "file.csv";

    {
        std::ofstream out(filePath);
        out << "a;b;c\n";
        out << "# this is a comment\n";
        out << "1;2;3\n";
        out << "4;5;6#here too\n";
        out << "4;5;6 #here too\n";
        out << "# and this is a another comment\n";
    }

    auto result = File::ReadCSV(filePath.string(), ";");
    REQUIRE(result.size() == 4);
    CHECK(result[0] == std::vector<std::string>{"a","b","c"});
    CHECK(result[1] == std::vector<std::string>{"1","2","3"});
    CHECK(result[2] == std::vector<std::string>{"4","5","6"});
    CHECK(result[3] == std::vector<std::string>{"4","5","6 "});
}

TEST_CASE("[File] ReadCSV: skips empty and whitespace lines") {
    TempDir tmp;
    fs::path filePath = tmp.path / "file.csv";

    {
        std::ofstream out(filePath);
        out << "\n";
        out << "#a comment \n";
        out << "a;  b;c  \n";
        out << "   \t   \n";
        out << "  1;\t2;3\t\n";
    }

    auto result = File::ReadCSV(filePath.string(), ";");
    REQUIRE(result.size() == 2);
    CHECK(result[0] == std::vector<std::string>{"a","  b","c  "});
    CHECK(result[1] == std::vector<std::string>{"  1","\t2","3\t"});
}

TEST_CASE("[File] ReadCSV: delimiter") {
    TempDir tmp;
    fs::path filePath = tmp.path / "file.csv";

    {
        std::ofstream out(filePath);
        out << "a,b,c\n";
        out << "1,2,3\n";
    }

    auto result = File::ReadCSV(filePath.string(), ",");
    REQUIRE(result.size() == 2);
    CHECK(result[0] == std::vector<std::string>{"a","b","c"});
    CHECK(result[1] == std::vector<std::string>{"1","2","3"});
}

}

TEST_SUITE("[File] EncodeToUTF8BOM") {

TEST_CASE("[File] EncodeToUTF8BOM") {
    TempDir tmp;
    fs::path filePath = tmp.path / "file.txt";

    {
        std::ofstream out(filePath, std::ios::binary);
        REQUIRE(out.is_open());
        File::EncodeToUTF8BOM(out);
    }

    std::ifstream in(filePath, std::ios::binary);
    REQUIRE(in.is_open());

    std::vector<unsigned char> bytes(3);
    in.read(reinterpret_cast<char*>(bytes.data()), 3);

    CHECK(bytes[0] == 0xEF);
    CHECK(bytes[1] == 0xBB);
    CHECK(bytes[2] == 0xBF);
}

TEST_CASE("[File] EncodeToUTF8BOM: invalid stream") {
    std::ofstream out;
    CHECK_THROWS_AS(File::EncodeToUTF8BOM(out), std::runtime_error);
}

}