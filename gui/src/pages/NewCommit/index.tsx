import React from "react";
import FileList from "./FileList";
import PackOption from "./PackOption";

const NewCommit: React.FC = () => {
    return (
        <div className="w-full h-full flex flex-col bg-white">
            <FileList />
            <PackOption />
        </div>
    );
}
    
export default NewCommit;