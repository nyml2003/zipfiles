import TreeSelector from './TreeSelector';
import React, { useState } from 'react';

interface Props {
}

const ExplorerPage: React.FC = () => {
  const [selectedItems, setSelectedItems] = useState<string[]>([]);
  const [currentPath, setCurrentPath] = useState<string>('app');

  const handleSelect = (paths: string[]) => {
    //按字典序排序
    setSelectedItems(paths.sort());
  };

  return (
    <div className='h-full flex'>
      <div className='w-1/3 border-r border-gray-200 overflow-scroll'>
        <TreeSelector onSelect={handleSelect} currentPath={currentPath} setCurrentPath={setCurrentPath} />
      </div>
      <div className='w-2/3 p-4'>
        {selectedItems.length > 0 ? (
          <div>
            <h3>已选中:</h3>
            <ul>
              {selectedItems.map((item, index) => (
                <li key={index}>{item}</li>
              ))}
            </ul>
          </div>
        ) : (
          <div>请选择一个项目</div>
        )}
      </div>
    </div>
  );
};

export default ExplorerPage;
